extern crate uavcan;

use std::sync::mpsc;
use std::sync::mpsc::{Sender, Receiver};
use std::sync::Arc;
use std::{thread, time};

use self::uavcan::transfer::TransferInterface;
use self::uavcan::transfer::TransferFrame;
use self::uavcan::transfer::TransferFrameID;

use self::uavcan::NodeID;
use self::uavcan::NodeConfig;
use self::uavcan::SimpleNode;
use self::uavcan::Node;
use self::uavcan::transfer::IOError;

#[derive(Clone, Debug)]
pub struct CanFrame {
    id: TransferFrameID,
    length: usize,
    data: [u8; 8],
}

impl TransferFrame for CanFrame {
    const MAX_DATA_LENGTH: usize = 8;

    fn new(id: TransferFrameID) -> CanFrame {
        CanFrame{id: id, length: 0, data: [0; 8]}
    }

    fn set_data_length(&mut self, length: usize) {
        assert!(length <= CanFrame::MAX_DATA_LENGTH);
        self.length = length;
    }

    fn data(&self) -> &[u8] {
        &self.data[0..self.length]
    }

    fn data_as_mut(&mut self) -> &mut[u8] {
        &mut self.data[0..self.length]
    }

    fn id(&self) -> TransferFrameID {
        self.id
    }
}


pub struct UavcanInterface {
    tx_frame : Sender<CanFrame>,
    rx_frame : Receiver<CanFrame>,
}

impl UavcanInterface {
     pub fn new() -> UavcanInterface {
         let (tx_frame, rx_frame): (Sender<CanFrame>, Receiver<CanFrame>) = mpsc::channel();

         UavcanInterface {
             tx_frame,
             rx_frame,
         }
     }
}

impl TransferInterface for UavcanInterface {
    type Frame = CanFrame;

    fn transmit(&self, frame: &Self::Frame) -> Result<(), IOError> {
        println!("TODO: transmit {:?}", frame);
        Ok(())
    }

    fn receive(&self) -> Option<Self::Frame> {
         None
    }
}


//pub fn get_tx_interface_channel() -> Sender<CanFrame> {
//    TX_INTERFACE.clone()
//}


pub fn init() -> UavcanInterface {
    let can_interface = UavcanInterface::new();
    let node_config = NodeConfig{id: Some(NodeID::new(115))};
    let node = Arc::new(SimpleNode::new(&can_interface, node_config));

    thread::spawn(move || {
        loop {
            while let Some(receive_res) = subscriber.receive() {
                let message = receive_res.unwrap();
                println!("Received node status frame: {:?}",  message);
            }
            thread::sleep(time::Duration::from_millis(10));
        }
    });

    thread::spawn(move || {
        loop {
            let now = time::SystemTime::now();
            let message = dsdl::uavcan::protocol::NodeStatus{
                uptime_sec: now.duration_since(start_time).unwrap().as_secs() as u32,
                health: u2::new(0),
                mode: u3::new(0),
                sub_mode: u3::new(0),
                vendor_specific_status_code: 0,
            };
            node.broadcast(message);
            thread::sleep(time::Duration::from_millis(1000));
        }
    });

    can_interface
}


