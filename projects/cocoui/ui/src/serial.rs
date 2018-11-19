extern crate serialport;

use std::{thread, time};
use std::io;

use self::serialport::SerialPort;
use self::serialport::SerialPortType;
use self::serialport::SerialPortSettings;

use com::UavcanInterface;
use com::CanFrame;

#[derive(Clone, Debug)]
struct Frame
{
    len: u8,
    data: [u8; 8],
    crc: u16,
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
        println!("Read {}", byte);
        self.buffer.push(byte)
    }

    pub fn decode(&mut self) -> Option<Frame> {
        println!("Decode {}", self.buffer.len());
        while self.buffer.len() >= 12 {
            println!("new decode {}", self.buffer.len());
            if self.buffer[0] != 'C' as u8 {
                 self.buffer.remove(0);
            }
            else {
                let mut data = [0 as u8; 8];
                for i in 0..8 {
                    data[i] = self.buffer[2 + i];
                }
                let crc = (self.buffer[10] as u16) + ((self.buffer[11] as u16) << 8);
                let f = Frame {
                    len: self.buffer[1],
                    data: data,
                    crc: crc,
                };
                self.buffer.drain(0..12);
                return Some(f);
            }
        }
        None
    }
}

struct SerialManager
{
    serial: Box<dyn SerialPort>,
    name: String,
    ith: UavcanInterface,
}

impl SerialManager
{
   pub fn new(ith: UavcanInterface, serial: Box<dyn SerialPort>) -> SerialManager {
       let name = serial.name().unwrap();
       println!("Port created {} ", name);
       let r = SerialManager {
           serial,
           name: name,
           ith,
       };
       r.start();
       r
   }

   pub fn name(&self) -> String {
       self.name.clone()
   }

   fn start(&self) {
       let mut serial = self.serial.try_clone().unwrap();
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
                           println!("Frame {:?} ", frame);
                       }
                   }
                   Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
                   Err(e) => eprintln!("{:?}", e),
               }
           }
       });
   }
}

fn find_serial_port(ith: UavcanInterface, opened_serial_port: &mut Vec<SerialManager>)
{
    if let Ok(ports) = serialport::available_ports() {
        match ports.len() {
            0 => println!("No ports found."),
            1 => println!("Found 1 port:"),
            n => println!("Found {} ports:", n),
        };
        for p in ports {
            println!("  {}", p.port_name);

            if !opened_serial_port.iter().any(|x| x.name() == p.port_name) {
                let mut settings: SerialPortSettings = Default::default();
                settings.baud_rate = 115200;
                
                match serialport::open_with_settings(&p.port_name, &settings) {
                    Ok(mut port) => {
                        println!("Open {}", &p.port_name);
                        let port = SerialManager::new(ith, port);
                        opened_serial_port.push(port)
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
    }
}

pub fn init(ith: UavcanInterface) {
    thread::spawn(move || {
        let mut opened_serial_port = Vec::new();

        //check port list every 2s
        loop {
            find_serial_port(ith, &mut opened_serial_port);
            thread::sleep( time::Duration::from_secs(2));
        }
    });
}
