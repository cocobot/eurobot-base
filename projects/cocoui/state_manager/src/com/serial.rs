extern crate serialport;

use super::ComInstance;

use std::sync::Arc;
use std::sync::Mutex;
use std::io;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

use std::{thread, time};

use self::serialport::SerialPort;
use self::serialport::SerialPortSettings;

use com::canars::CANFrame;

#[derive(Clone, Debug)]
struct Frame
{
    frame: CANFrame, 
    crc: u16,
}

impl Frame {
     fn new(id: u32, data: [u8; CANFrame::CAN_FRAME_MAX_DATA_LEN], data_len: u8, crc: u16) -> Frame {
         Frame {
             frame: CANFrame::new(id, data, data_len),
             crc,
         }
     }

     fn get_can_frame(self) -> CANFrame {
          self.frame
     }
}

struct FrameDecoder{
    buffer: Vec<u8>,
}

impl FrameDecoder {
    pub fn new() -> FrameDecoder {
        FrameDecoder {
            buffer: Vec::new()
        }
    }

    pub fn add_byte(&mut self, byte: u8) {
        self.buffer.push(byte)
    }

    pub fn decode(&mut self) -> Option<Frame> {
        while self.buffer.len() >= 16 {
            if self.buffer[0] != 'C' as u8 {
                self.buffer.remove(0);
            }
            else {
                let id = (self.buffer[1] as u32) + 
                    ((self.buffer[2] as u32) << 8) +
                    ((self.buffer[3] as u32) << 16) +
                    ((self.buffer[4] as u32) << 24);
                let mut data = [0 as u8; 8];
                for i in 0..8 {
                    data[i] = self.buffer[6 + i];
                }
                let crc = (self.buffer[14] as u16) + ((self.buffer[15] as u16) << 8);
                let f = Frame::new(
                    id,
                    data,
                    self.buffer[5],
                    crc,
                );
                self.buffer.drain(0..12);
                println!("Todo: check CRC");
                return Some(f);
            }
        }
        None
    }
}

struct SerialManager {
    serial: Box<dyn SerialPort>,
    name: String,
    opened_serial_port: Arc<Mutex<Vec<Box<SerialManager>>>>,
    com: ComInstance,
}

impl SerialManager {
    fn new(com: ComInstance, serial: Box<dyn SerialPort>, opened_serial_port : Arc<Mutex<Vec<Box<SerialManager>>>>) -> SerialManager {
        let sm = SerialManager {
            name: serial.name().unwrap(),
            serial,
            opened_serial_port,
            com,
        };
        sm.start();
        sm
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    fn start(&self) {
        let mut serial = self.serial.try_clone().unwrap();
        let opened_serial_port = self.opened_serial_port.clone();
        let com = self.com.clone();

        thread::spawn(move || {
            let mut serial_buf: Vec<u8> = vec![0; 1000];
            let mut decoder = FrameDecoder::new();
            loop {
                match serial.read(serial_buf.as_mut_slice()) {
                    Ok(t) => {
                        for b in serial_buf[..t].iter() {
                            decoder.add_byte(*b);
                        }
                        while let Some(frame) = decoder.decode() {
                            let timestamp =  SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
                            let timestamp = timestamp.as_secs() * 1_000 +
                                timestamp.subsec_nanos() as u64 / 1_000_000;

                            match com.lock() {
                                Ok(mut c) => {
                                    let mut node = c.get_node();
                                    drop(c);
                                    match node {
                                        Some(ref mut s) => s.lock().unwrap().handle_rx_frame(frame.get_can_frame(), timestamp),
                                        None => {},
                                    };
                                },
                                Err(e) => eprintln!("{:?}", e),
                            }
                        }
                    }
                    Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
                    Err(ref e) if e.kind() == io::ErrorKind::BrokenPipe => {
                        println!("Close {}", serial.name().unwrap());
                        match opened_serial_port.lock() {
                             Ok(mut list) => {
                                 list.iter().position(|x| x.name() == serial.name().unwrap()).map(|e| list.remove(e));
                                 drop(serial);
                                 return;
                             }
                             Err(_) => {},
                        }
                    },
                    Err(e) => eprintln!("{:?}", e),
                }
            }
        });
    }
}

fn find_serial_port(com: &mut ComInstance, opened_serial_port: &mut Arc<Mutex<Vec<Box<SerialManager>>>>) {
    if let Ok(ports) = serialport::available_ports() {
        for p in ports {
            match opened_serial_port.lock() {
                Ok(mut list) => { if !list.iter().any(|x| x.name() == p.port_name) {
                    let mut settings: SerialPortSettings = Default::default();
                    settings.baud_rate = 115200;

                    match serialport::open_with_settings(&p.port_name, &settings) {
                        Ok(mut port) => {
                            println!("Open {}", &p.port_name);
                            let port = SerialManager::new(com.clone(), port, opened_serial_port.clone());
                            list.push(Box::new(port));
                        },
                        Err(e) => {
                            eprintln!(
                                "Failed to open \"{}\". Error: {}",
                                p.port_name,
                                e
                                );
                        },
                    }
                }

                }
                Err(_) => {},
            }
        }
    }
}

pub fn init(com: ComInstance) {
    thread::spawn(move || {
        let mut opened_serial_port = Arc::new(Mutex::new(Vec::new()));
        let mut com = com;

        //check port list every 2s
        loop {
            find_serial_port(&mut com, &mut opened_serial_port);
            thread::sleep( time::Duration::from_secs(2));
        }
    });
}
