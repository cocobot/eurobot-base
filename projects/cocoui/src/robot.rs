extern crate tokio;
extern crate bytes;
extern crate colored;

use std::io;
use std::sync::mpsc::Sender;

use robot::colored::*;
use robot::tokio::prelude::*;

use protocol::{Packet, Protocol};
use ui;

#[derive(Clone, Debug)]
pub struct AsservData {
    pub target: f64,
    pub current: f64,

    pub speed: f64,

    pub ramp_output: f64,
    pub ramp_speed_target: f64,

    pub pid_output: f64,
    pub pid_p: f64,
    pub pid_i: f64,
    pub pid_d: f64,
}

impl AsservData {
    pub fn new() -> AsservData {
        AsservData {
            target: 0.0,
            current: 0.0,
            speed: 0.0,
            ramp_output: 0.0,
            ramp_speed_target: 0.0,
            pid_output: 0.0,
            pid_p: 0.0,
            pid_i: 0.0,
            pid_d: 0.0,
        }
    }
}

#[derive(Clone, Debug)]
pub struct RobotData {
    pub x_mm: f64,
    pub y_mm: f64,
    pub a_deg: f64,
    pub score: i32,
    pub elapsed_time_s: i32,
    pub battery_mv: i32,
    pub asserv_distance: AsservData,
    pub asserv_angular: AsservData,
    pub pathfinder: Vec<Vec<usize>>,
}

impl RobotData {
    pub fn new() -> RobotData {
         RobotData {
              x_mm: 0.0,
              y_mm: 0.0,
              a_deg: 0.0,
              score: 0,
              elapsed_time_s: 0,
              battery_mv: 0,
              asserv_distance: AsservData::new(),
              asserv_angular: AsservData::new(),
              pathfinder: Vec::new(),
         }
    }
}


pub struct Robot {
    protocol: Protocol,
    tx_rdata: Sender<RobotData>, 
    data: RobotData,
}

impl Robot {
     pub fn new(protocol : Protocol, tx_rdata: Sender<RobotData>) -> Robot {
         let r = Robot {
             protocol,
             tx_rdata,
             data: RobotData::new(),
         };

         r
     }
}

impl Future for Robot {
    type Item = ();
    type Error = io::Error;

    fn poll(&mut self) -> Poll<(), io::Error> {
        // Read new packets from the socket
        while let Async::Ready(packet) = self.protocol.poll()? {
            if let Some(packet) = packet {
                match packet {
                    Packet::Unknown{pid} => {
                        eprintln!("{} 0x{:X}", "Unknown PID received:".red(), pid);
                    },
                    Packet::Position{x, y, a} => {
                         self.data.x_mm = x;
                         self.data.y_mm = y;
                         self.data.a_deg = a;
                    },
                    Packet::AsservDistance{
                        position_target, 
                        position_current, 
                        ramp_output, 
                        ramp_speed_target, 
                        speed_current, 
                        pid_output, 
                        pid_p, 
                        pid_i, 
                        pid_d, 
                    } => {
                         self.data.asserv_distance.target = position_target;
                         self.data.asserv_distance.current = position_current;
                         self.data.asserv_distance.speed = speed_current;
                         self.data.asserv_distance.ramp_output = ramp_output;
                         self.data.asserv_distance.ramp_speed_target = ramp_speed_target;
                         self.data.asserv_distance.pid_output = pid_output;
                         self.data.asserv_distance.pid_p = pid_p;
                         self.data.asserv_distance.pid_i = pid_i;
                         self.data.asserv_distance.pid_d = pid_d;
                    },
                    Packet::AsservAngular{
                        position_target, 
                        position_current, 
                        ramp_output, 
                        ramp_speed_target, 
                        speed_current, 
                        pid_output, 
                        pid_p, 
                        pid_i, 
                        pid_d, 
                    } => {
                         self.data.asserv_angular.target = position_target;
                         self.data.asserv_angular.current = position_current;
                         self.data.asserv_angular.speed = speed_current;
                         self.data.asserv_angular.ramp_output = ramp_output;
                         self.data.asserv_angular.ramp_speed_target = ramp_speed_target;
                         self.data.asserv_angular.pid_output = pid_output;
                         self.data.asserv_angular.pid_p = pid_p;
                         self.data.asserv_angular.pid_i = pid_i;
                         self.data.asserv_angular.pid_d = pid_d;
                    },
                    Packet::GameState{secondary: _, positive_color: _, battery_mv, elapsed_time_s, score} => {
                         self.data.battery_mv = battery_mv;
                         self.data.elapsed_time_s = elapsed_time_s;
                         self.data.score = score;
                    },
                    Packet::Pathfinder{length, width, grid} => {
                        let mut iter = grid.iter();
                        let mut pathfinder = vec![];
                        for _i in 0..length {
                            let mut v : Vec<usize> = vec![0; width];
                            for j in 0..width {
                                let nxt = iter.next();
                                if nxt.is_some() {
                                    v[j] = *nxt.unwrap();
                                }
                            }
                            pathfinder.push(v);
                        }

                        self.data.pathfinder = pathfinder;
                    },

                    _ => {
                        eprintln!("{} {:?}", "Unhandled received packet:".yellow(), packet);
                    }
                }
                self.tx_rdata.send(self.data.clone()).unwrap();
                ui::update();
            } else {
                // EOF was reached. The remote client has disconnected. There is
                // nothing more to do.
                return Ok(Async::Ready(()));
            }
        }

        // As always, it is important to not just return `NotReady` without
        // ensuring an inner future also returned `NotReady`.
        //
        // We know we got a `NotReady` from either `self.rx` or `self.lines`, so
        // the contract is respected.
        Ok(Async::NotReady)
    }
}
