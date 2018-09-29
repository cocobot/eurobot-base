extern crate tokio;
extern crate bytes;

use std::io;
use protocol::bytes::{BytesMut, Bytes, Buf};
use protocol::tokio::prelude::*;
use protocol::tokio::net::TcpStream;

pub struct Protocol {
    socket: TcpStream,
    rd: BytesMut,
    
    //packet decoder
    robot_identifier_found: bool,
}

impl Protocol {
    pub fn new(socket: TcpStream) -> Self {
        Protocol {
            socket,
            rd: BytesMut::new(),
            robot_identifier_found: false,
        }
    }

    fn crc16_update(crc: u16, byte: u8) -> u16 {
        let mut crc = crc ^ (byte as u16);

        for _i in 0..8 {
            if crc & 1 != 0 {
                crc = (crc >> 1) ^ 0xA001;
            }
            else {
                crc = crc >> 1;
            }
        }

        crc
    }
}

impl Stream for Protocol {
    type Item = Packet;
    type Error = io::Error;


    fn poll(&mut self) -> Result<Async<Option<Self::Item>>, Self::Error> {
        // First, read any new data that might have been received
        // off the socket
        //
        // We track if the socket is closed here and will be used
        // to inform the return value below.
        let sock_closed = self.fill_read_buf()?.is_ready();

        let mut packet_pid = None;
        let mut packet_to_split = 0;

        if !self.robot_identifier_found {
            // Now, try finding lines
            let pos = self.rd.windows(1)
                .position(|bytes| bytes == b"\n");

            if let Some(pos) = pos {
                let mut line = self.rd.split_to(pos + 1);
                line.split_off(pos);
                self.robot_identifier_found = true;
                return Ok(Async::Ready(Some(Packet::SelectRobot{ secondary: false })));
            }
        }
        else {
            //decode header
            let magic = self.rd.get(0);
            if magic.is_some() {
                if *magic.unwrap() != 0xC0 {
                    //bad magic header. Skip first byte
                    packet_to_split= 1;

                    //rerun scheduler with new buffer
                    task::current().notify();
                }
                else {
                    //check CRC
                    if self.rd.len() >= 7 {
                        //read crc from buffer
                        let protocol_crc = (*self.rd.get(5).unwrap() as u16) | ((*self.rd.get(6).unwrap() as u16) << 8);

                        //compute received crc
                        let mut computed_crc = 0xFFFF;
                        for i in 0..5 {
                            computed_crc = Protocol::crc16_update(computed_crc, *self.rd.get(i).unwrap());
                        }
                        if protocol_crc != computed_crc {
                            //bad header crc. Skip first byte
                            packet_to_split = 1;

                            //rerun scheduler with new buffer
                            task::current().notify();    
                        }
                        else {
                            //decode valid header
                            let pid = (*self.rd.get(1).unwrap() as usize) | ((*self.rd.get(2).unwrap() as usize) << 8);
                            let len = (*self.rd.get(3).unwrap() as usize) | ((*self.rd.get(4).unwrap() as usize) << 8);

                            //read data
                            if self.rd.len() >= 7 + len + 2 {
                                let crc_pos = len + 7;
                                //read crc from buffer
                                let protocol_crc = (*self.rd.get(crc_pos).unwrap() as u16) | ((*self.rd.get(crc_pos + 1).unwrap() as u16) << 8);

                                //compute received crc
                                let mut computed_crc = 0xFFFF;
                                for i in 7..(7 + len) {
                                    computed_crc = Protocol::crc16_update(computed_crc, *self.rd.get(i).unwrap());
                                }

                                //Do not check for DATA CRC. It seems that it has never been
                                //implemented in libcocobot...
                                if protocol_crc == computed_crc {
                                    println!("We are lucky ! The only packet with valid CRC !");
                                }

                                packet_pid = Some(pid);
                                packet_to_split = len + 7 + 2;
                            }
                            else {
                                //TODO: use timeout
                            }
                        }
                    }
                }
            }
        }

        if packet_to_split > 0 {
            let mut bytes = self.rd.split_to(packet_to_split).freeze();
            if packet_pid.is_some() {
                //remove header
                bytes.split_to(7); 

                //parse packet
                let packet = Protocol::parse(packet_pid.unwrap(), bytes);
                return Ok(Async::Ready(Some(packet)));
            }
        }

        if sock_closed {
            Ok(Async::Ready(None))
        } else {
            Ok(Async::NotReady)
        }
    }
}

impl Protocol {
    fn fill_read_buf(&mut self) -> Result<Async<()>, io::Error> {
        loop {
            // Ensure the read buffer has capacity.
            //
            // This might result in an internal allocation.
            self.rd.reserve(1024);

            // Read data into the buffer.
            //
            // The `read_buf` fn is provided by `AsyncRead`.
            let n = try_ready!(self.socket.read_buf(&mut self.rd));

            if n == 0 {
                return Ok(Async::Ready(()));
            }
        }
    }

    fn parse(pid: usize, bytes: Bytes) -> Packet {
        let mut bytes = io::Cursor::new(bytes);
        match pid {
            0x8000 => {
                Packet::PositionDebug {
                    x: bytes.get_f32_le() as f64,
                    y: bytes.get_f32_le() as f64,
                    a: bytes.get_f32_le() as f64,
                }
            },
            _ => Packet::Unknown { pid }
        }
    }
}

#[derive(Debug)]
pub enum Packet {
    PositionDebug {x: f64, y: f64, a: f64},

    //specials
    SelectRobot {secondary: bool},
    Unknown {pid: usize},
}
