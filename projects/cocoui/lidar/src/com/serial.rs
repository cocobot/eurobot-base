use super::Com;

use std::{thread, time};
use canars::CANFrame;
use crossbeam_channel::Receiver;
use rppal::uart::{Parity, Uart};

#[derive(Clone, Debug)]
struct Frame {
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
        } else {
            crc = crc >> 1;
        }
    }

    crc
}

struct FrameDecoder {
    buffer: Vec<u8>,
    error: usize,
}

impl FrameDecoder {
    pub fn new() -> FrameDecoder {
        FrameDecoder { buffer: Vec::new(), error: 0, }
    }

    fn get_error(&self) -> usize {
        self.error
    }

    pub fn add_byte(&mut self, byte: u8) {
        self.buffer.push(byte)
    }

    pub fn decode(&mut self) -> Option<Frame> {
        while self.buffer.len() >= 16 {
            if self.buffer[0] != 'C' as u8 {
                debug!("Link out of sync: {:?}", &self.buffer);
                self.buffer.remove(0);
                self.error += 1;
            } else {
                let id = (self.buffer[1] as u32)
                    + ((self.buffer[2] as u32) << 8)
                    + ((self.buffer[3] as u32) << 16)
                    + ((self.buffer[4] as u32) << 24);
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
                    self.error += 1;
                } else {
                    let f = Frame::new(id, data, self.buffer[5], crc);
                    self.buffer.drain(0..16);
                    self.error = 0;
                    return Some(f);
                }
            }
        }
        None
    }
}

struct FrameEncoder {}

impl FrameEncoder {
    pub fn new() -> FrameEncoder {
        FrameEncoder {}
    }

    pub fn encode(&self, frame: &CANFrame) -> Vec<u8> {
        let mut buffer = Vec::new();

        let id = frame.get_id();
        let len = frame.get_data_len();
        let data = frame.get_data();

        buffer.push('C' as u8);
        buffer.push(((id >> 0) & 0xFF) as u8);
        buffer.push(((id >> 8) & 0xFF) as u8);
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

pub fn init(com: Com, tx_can: Receiver<CANFrame>) {
    thread::spawn(move || {
        loop {
            let uart = Uart::with_path("/dev/canars", 115_200, Parity::None, 8, 1);
            if let Ok(mut uart) = uart {
                uart.set_read_mode(1, time::Duration::from_millis(5)).unwrap();

                let encoder = FrameEncoder::new();
                let mut decoder = FrameDecoder::new();
                let mut fail = false;
                while !fail  {
                    let mut data = [0u8; 1];
                    if let Ok(_) = uart.read(&mut data) {
                        decoder.add_byte(data[0]);
                        while let Some(frame) = decoder.decode() {
                            com.handle_rx_frame(frame.get_can_frame());
                        }
                    }

                    while let Ok(frame) = tx_can.try_recv() {
                        debug!("TX: {:?}", frame);
                        let enc = encoder.encode(&frame);
                        if let Err(v) = uart.write(&enc[..]) {
                            error!("write: {:?}", v);
                        }
                    }

                    if decoder.get_error() > 32 {
                        fail = true;
                    }
                }
            }
            thread::sleep(time::Duration::from_millis(250));
        }
    });
}

