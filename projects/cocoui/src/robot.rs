extern crate tokio;
extern crate bytes;
extern crate colored;

use std::io;
use std::sync::mpsc::Sender;

use robot::colored::*;
use robot::tokio::prelude::*;

use protocol::{Packet, Protocol};
use ui;

#[derive(Clone)]
pub struct RobotData {
    pub x_mm: f64,
    pub y_mm: f64,
    pub a_deg: f64,
    pub score: i32,
    pub elapsed_time_s: i32,
    pub battery_mv: i32,
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
   //                     eprintln!("{} 0x{:X}", "Unknown PID received:".red(), pid);
                    },
                    Packet::Position{x, y, a} => {
                         self.data.x_mm = x;
                         self.data.y_mm = y;
                         self.data.a_deg = a;
                    },
                    Packet::GameState{secondary: _, positive_color: _, battery_mv, elapsed_time_s, score} => {
                         self.data.battery_mv = battery_mv;
                         self.data.elapsed_time_s = elapsed_time_s;
                         self.data.score = score;
                    },
                    _ => {
 //                       eprintln!("{} {:?}", "Unhandled received packet:".yellow(), packet);
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
