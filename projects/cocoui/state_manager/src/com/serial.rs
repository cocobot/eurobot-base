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
use self::serialport::SerialPortType;

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

fn crc16_update(crc: u16, a: u8) -> u16 {
    let mut crc = crc ^ (a as u16);
    for _i in 0..8 {
        if (crc & 1) != 0 {
            crc = (crc >> 1) ^ 0xA001;
        }
        else {
            crc = crc >> 1;
        }
    }

    crc
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
                debug!("Link out of sync: {:?}", &self.buffer);
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
                let mut computed_crc = 0xFFFF;
                for i in 0..14 {
                    computed_crc = crc16_update(computed_crc, self.buffer[i]);
                }
                if computed_crc != crc {
                    self.buffer.remove(0);
                }
                else {
                    let f = Frame::new(
                        id,
                        data,
                        self.buffer[5],
                        crc,
                        );
                    self.buffer.drain(0..16);
                    return Some(f);
                }
            }
        }
        None
    }
}

struct FrameEncoder{
}

impl FrameEncoder {
    pub fn new() -> FrameEncoder {
        FrameEncoder {
        }
    }

    pub fn encode(&self, frame: &CANFrame) -> Vec<u8> {
        let mut buffer = Vec::new();

        let id = frame.get_id();
        let len = frame.get_data_len();
        let data = frame.get_data();

        buffer.push('C' as u8);
        buffer.push(((id >>  0) & 0xFF) as u8);
        buffer.push(((id >>  8) & 0xFF) as u8);
        buffer.push(((id >> 16) & 0xFF) as u8);
        buffer.push(((id >> 24) & 0xFF) as u8);
        buffer.push(len as u8);
        for i in 0..8 {
            buffer.push(data[i]);
        }
        let mut computed_crc = 0xFFFF;
        for i in 0..14 {
            computed_crc = crc16_update(computed_crc, buffer[i]);
        }
        buffer.push((computed_crc & 0xFF) as u8);
        buffer.push(((computed_crc >> 8) & 0xFF) as u8);

        buffer
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

    pub fn send(&mut self, buffer: Vec<u8>) {
        match self.serial.write_all(&buffer) {
             Ok(_) => {},
             Err(e) => {error!("write_all: {:?}", e);},
        };
        match self.serial.flush() {
             Ok(_) => {},
             Err(e) => {error!("flush: {:?}", e);},
        }
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
                                Err(e) => error!("Com lock: {:?}", e),
                            }
                        }
                    }
                    Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
                    Err(ref e) if e.kind() == io::ErrorKind::BrokenPipe => {
                        debug!("Close {}", serial.name().unwrap());
                        match opened_serial_port.lock() {
                             Ok(mut list) => {
                                 list.iter().position(|x| x.name() == serial.name().unwrap()).map(|e| list.remove(e));
                                 drop(serial);
                                 return;
                             }
                             Err(_) => {},
                        }
                    },
                    Err(e) => error!("Serial read: {:?}", e),
                }
            }
        });
    }
}

fn find_serial_port(com: &mut ComInstance, opened_serial_port: &mut Arc<Mutex<Vec<Box<SerialManager>>>>) {
    if let Ok(ports) = serialport::available_ports() {
        for p in ports {
            match p.port_type {
                SerialPortType::UsbPort(ref info)  => {
                    if info.serial_number.is_none() {
                        continue;
                    }
                    if info.serial_number.as_ref().unwrap() != "A105TLE3" {
                        continue;
                    }
                },
                _ => {continue },
            }
            match opened_serial_port.lock() {
                Ok(mut list) => { if !list.iter().any(|x| x.name() == p.port_name) {
                    let mut settings: SerialPortSettings = Default::default();
                    settings.baud_rate = 115200;

                    match serialport::open_with_settings(&p.port_name, &settings) {
                        Ok(port) => {
                            debug!("Open {}", &p.port_name);
                            let port = SerialManager::new(com.clone(), port, opened_serial_port.clone());
                            list.push(Box::new(port));
                        },
                        Err(e) => {
                            error!(
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

fn write_thread(com: ComInstance, opened_serial_port: Arc<Mutex<Vec<Box<SerialManager>>>>) {
    let mut com = com.lock().unwrap();
    let node = com.get_node().unwrap();
    drop(com);
    thread::spawn(move || {
        let encoder = FrameEncoder::new();
        loop {
            let mut instance = node.lock().unwrap();
            
            while let Some(frame) = instance.pop_tx_queue() {
                debug!("TX {:?}", frame);
                match opened_serial_port.lock() {
                    Ok(mut list) => { 
                        for port in list.iter_mut() {
                            port.send(encoder.encode(&frame));
                        }
                    },
                    Err(e) => {
                        error!("Error: {}", e);
                    },
                }
            }

            drop(instance);

            thread::sleep(time::Duration::from_millis(20));
        }
    });
}


pub fn init(com: ComInstance) {
    thread::spawn(move || {
        let mut opened_serial_port = Arc::new(Mutex::new(Vec::new()));
        let mut com = com;

        write_thread(com.clone(), opened_serial_port.clone());

        //check port list every 2s
        loop {
            find_serial_port(&mut com, &mut opened_serial_port);
            thread::sleep( time::Duration::from_secs(2));
        }
    });
}
