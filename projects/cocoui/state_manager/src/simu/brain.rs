extern crate canars;

use std::io::Write;
use std::io::{BufRead, BufReader};
use std::process::ChildStdin;
use std::process::{Command, Stdio};
use std::sync::Arc;
use std::sync::Mutex;

use self::canars::CANFrame;
use crate::com::Com;
use crate::simu::physics::PhysicsInstance;


#[derive(Copy, Clone)]
pub struct Timer {
    freq: i32,
    count: u32,
}

impl Timer {
    pub fn new() -> Timer {
        Timer { freq: 0, count: 0 }
    }

    pub fn init(&mut self, freq: i32) {
        self.freq = freq;
    }

    pub fn step(&mut self, step_time: u32) {
        if self.freq < 0 {
            //counter only
            self.count += step_time * ((-self.freq) as u32) / 1000;
        }
    }

    pub fn get_count(&self) -> u32 {
        self.count
    }
}

pub type BrainInstance = Arc<Mutex<Brain>>;

pub struct Brain {
    com: Com,
    path: String,
    timers: [Timer; Brain::TIMER_COUNT],
    stdin: Option<ChildStdin>,
}

impl Brain {
    const TIMER_COUNT: usize = 5;

    pub fn new(com: Com, path: String) -> BrainInstance {
        let brain = Brain {
            com,
            path,
            timers: [Timer::new(); Brain::TIMER_COUNT],
            stdin: None,
        };

        let instance = Arc::new(Mutex::new(brain));

        instance
    }

    fn timer_init(&mut self, module_id: usize, freq: i32) {
        self.timers[module_id].init(freq);
    }

    fn get_path(&self) -> &str {
        &self.path
    }

    fn parse(&mut self, line: &str) -> bool {
        let bytes = line.as_bytes();
        if bytes[0] != '>' as u8 {
            //print debug from simulator in console
            debug!("{}", line.trim());
        } else {
            let split = line[1..].trim().split("/");
            let tokens: Vec<&str> = split.collect();
            let module: Vec<&str> = tokens.get(0).unwrap().split(":").collect();

            if module.len() < 2 {
                debug!("? {:?}", &line[1..]);
                return false;
            }

            let module_name = module.get(0).unwrap();
            let module_id = module.get(1).unwrap().to_owned().parse::<usize>().unwrap();

            match module_name.as_ref() {
                "ARCH" => {
                    let cmd = tokens.get(1).unwrap().as_ref();
                    match cmd {
                        "RUN" => info!("Simulation started ({})", self.get_path()),
                        "REBOOT" => { 
                            return true
                        },
                        _ => warn!("Unexpected ARCH command: '{}'", cmd),
                    }
                }
                "TIMER" => {
                    let tokens: Vec<&str> = tokens.get(1).unwrap().split(":").collect();
                    let cmd = tokens.get(0).unwrap().as_ref();
                    match cmd {
                        "INIT" => {
                            let freq = tokens.get(1).unwrap().to_owned().parse::<i32>().unwrap();
                            self.timer_init(module_id, freq);
                        }
                        _ => warn!("Unexpected TIMER command: '{}'", cmd),
                    }
                }
                "CAN" => {
                    let tokens: Vec<&str> = tokens.get(1).unwrap().split(":").collect();

                    let mut data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN] =
                        [0; CANFrame::CAN_FRAME_MAX_DATA_LEN];
                    for i in 0..8 {
                        data[i] = u8::from_str_radix(tokens.get(i + 2).unwrap(), 16).unwrap();
                    }

                    let frame = CANFrame::new(
                        u32::from_str_radix(tokens.get(0).unwrap(), 16).unwrap(),
                        data,
                        u8::from_str_radix(tokens.get(1).unwrap(), 16).unwrap(),
                    );

                    self.com.handle_rx_frame(frame);
                }

                _ => warn!("Unexpected command: {}", line),
            }
        }

        false
    }

    pub fn start(instance: BrainInstance, physics: PhysicsInstance) {
        let mut locked_instance = instance.lock().unwrap();
        info!("Start simulation: {}", locked_instance.get_path());
        let child = Command::new(locked_instance.get_path().to_owned())
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .spawn().unwrap();
        locked_instance.stdin = Some(child.stdin.unwrap());
        drop(locked_instance);

        let stdout = child.stdout.unwrap();
        let th_instance = instance.clone();
        std::thread::spawn(move || {
            let mut reader = BufReader::new(stdout);
            loop {
                let mut line = String::new();
                if reader.read_line(&mut line).is_ok() {
                    if line.len() != 0 {
                        let mut locked_instance = th_instance.lock().unwrap();
                        let reboot_me = locked_instance.parse(&line);
                        drop(locked_instance);

                        if reboot_me {
                            let locked_instance = th_instance.lock().unwrap();
                            let path = locked_instance.get_path().to_owned();
                            let com = locked_instance.com.clone();
                            warn!("Stop simulation: {}", path);
                            drop(locked_instance);
                            let mut ph = physics.lock().unwrap();
                            ph.remove_brain(th_instance);
                            drop(ph);

                            //create new instance
                            let brain = Brain::new(com, path);
                            Brain::start(brain.clone(), physics.clone());
                            physics.lock().unwrap().add_brain(brain);
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        });
    }

    pub fn send(&mut self, module: &str, id: usize, data: String) {
        match self.stdin.as_mut() {
            Some(stdin) => {
                match stdin.write_all(format!(">{}:{}/{}\n", module, id, data).as_bytes()) {
                    Ok(_) => {}
                    Err(e) => {
                        error!("write_all: {:?}", e);
                    }
                }
            }
            _ => {}
        }
    }

    pub fn step(&mut self, step_time: u32) {
        //update timers
        for i in 0..Brain::TIMER_COUNT {
            let last_count = self.timers[i].get_count();
            self.timers[i].step(step_time);

            if last_count != self.timers[i].get_count() {
                self.send("TIMER", i, format!("CNT={}", self.timers[i].get_count()));
            }
        }
    }

    pub fn can_tx(&mut self, frame: &CANFrame) {
        self.send(
            "CAN",
            1,
            format!(
                "{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}",
                frame.get_id(),
                frame.get_data_len(),
                frame.get_data()[0],
                frame.get_data()[1],
                frame.get_data()[2],
                frame.get_data()[3],
                frame.get_data()[4],
                frame.get_data()[5],
                frame.get_data()[6],
                frame.get_data()[7]
            ),
        );
    }
}
