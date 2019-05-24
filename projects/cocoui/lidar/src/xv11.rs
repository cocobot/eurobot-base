use std::{thread, time};
use rppal::uart::{Parity, Uart};

use std::sync::Arc;
use std::sync::Mutex;

const THRESHOLD : u16 = 0x80;


pub struct XV11 {
    values: [Option<u16>; 360]
}


impl XV11 {
    pub fn new() -> Arc<Mutex<XV11>> {
        let instance = XV11 {
           values: [None; 360], 
        };

        Arc::new(Mutex::new(instance))
    }

    fn set_values(&mut self, array: [Option<u16>; 360]) {
        self.values = array;
    }

    pub fn get_angle(&self, angle: usize) -> Option<u16> {
        self.values[angle % 360]
    }

    pub fn start(instance: &mut Arc<Mutex<XV11>>) {
        let instance = instance.clone();
        
        thread::spawn(move || {
            let mut uart = Uart::new(115_200, Parity::None, 8, 1).unwrap();
            uart.set_read_mode(1, time::Duration::default()).unwrap();
            let mut buffer : Vec<u8> = Vec::new();
            let mut values : [Option<u16>; 360] = [None; 360];
            loop {
                let mut data = [0u8; 1];
                if let Ok(_) = uart.read(&mut data) {
                    buffer.push(data[0]);


                    while buffer.len() > 0 {
                        //wait start byte
                        if buffer[0] != 0xFA {
                            buffer.remove(0);
                            break;
                        }

                        if buffer.len() < 22 {
                            break;
                        }

                        let idx = buffer[1] as usize;
                        let _speed = (buffer[2] as usize) | ((buffer[3] as usize) << 8);

                        for i in 0..4 {
                            let mut val = None; 
                            if (buffer[4 + i * 4 + 1] & 0xC0) == 0 {
                                let mut distance = buffer[4 + i * 4] as u16;
                                distance |= ((buffer[4 + i * 4 + 1] as u16) & 0x3F) << 8;

                                let mut signal = buffer[4 + i * 4 + 2] as u16;
                                signal |= (buffer[4 + i * 4 + 3] as u16) << 8;

                                if signal > THRESHOLD {
                                    val = Some(distance);
                                }
                            }
                            values[idx - 0xA0 + i] = val;
                        }

                        buffer.drain(0..21);

                        let mut locked = instance.lock().unwrap();
                        locked.set_values(values);
                        drop(locked);
                    }

                }

            }
        });
    }
}

