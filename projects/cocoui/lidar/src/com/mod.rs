extern crate dsdl;

pub mod serial;

use crossbeam_channel::unbounded;
use crossbeam_channel::{Receiver, Sender};
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

use canars::CANFrame;
use canars::Instance;
use canars::Node;
use canars::RxTransfer;
use canars::TransferType;
use crate::ComData;

pub struct ComHandler {}

impl ComHandler {
    fn new() -> ComHandler {
        ComHandler {}
    }
}

impl Node<ComData> for ComHandler {
    fn should_accept_transfer(
        &self,
        _state: &ComData,
        data_type_signature: &mut u64,
        data_type_id: u16,
        _transfer_type: TransferType,
        source_node_id: u8,
    ) -> bool {
        //Indicate canars if trame is useful or not
        if dsdl::uavcan::cocobot::GameState::check_id(data_type_id) {
            dsdl::uavcan::cocobot::GameState::set_signature(data_type_signature);
            true
        }
        else {
            false
        }
    }

    fn on_transfer_reception(&self, state: &mut ComData, xfer: &RxTransfer) {
        //Route packet depeding on id
        if dsdl::uavcan::cocobot::GameState::check_id(xfer.get_data_type_id()) {
            let gstate = dsdl::uavcan::cocobot::GameState::decode(xfer).unwrap();
            if state.start.elapsed().as_secs() < 15 {
                state.score.set_battery((gstate.battery as f32) / 1000.0);
            }
            else {
                state.score.set_score(gstate.score as usize);
            }
        } else {
            error!(
                "Xfer accepted but not implemented: {:?}",
                xfer.get_data_type_id()
            );
        }
    }
}

#[derive(Clone)]
pub struct Com {
    receive_can_frame: Sender<(u64, CANFrame)>,
    send_can_frame: Sender<CANFrame>,
}

impl Com {
    fn new(
        receive_can_frame: Sender<(u64, CANFrame)>,
        send_can_frame: Sender<CANFrame>,
    ) -> Com {
        //Allocation for com structure
        Com {
            receive_can_frame,
            send_can_frame,
        }
    }

    pub fn send_can_frame(&self, frame: CANFrame) {
        match self.send_can_frame.send(frame) {
            Ok(_) => {}
            Err(e) => {
                error!("send_can_frame: {:?}", e);
            }
        }
    }

    pub fn handle_rx_frame(&self, frame: CANFrame) {
        let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
        let timestamp = timestamp.as_secs() * 1_000 + timestamp.subsec_nanos() as u64 / 1_000_000;

        match self.receive_can_frame.send((timestamp, frame)) {
            Ok(_) => {}
            Err(e) => {
                error!("receive_can_frame: {:?}", e);
            }
        }
    }
}

pub fn init(node_id: u8, data: ComData) -> (Com, Receiver<CANFrame>) {
    //Canars node initialisation
    let mut node = Instance::init(ComHandler::new(), data);
    node.set_local_node_id(node_id);

    //protect node with mutex
    let node = Arc::new(Mutex::new(node));

    //create channels for communication
    let (tx_send_can_frame, rx_send_can_frame) = unbounded();
    let (tx_receive_can_frame, rx_receive_can_frame) = unbounded();

    //Com initialisation
    let com = Com::new(tx_receive_can_frame, tx_send_can_frame);

    //handle cleanup and send messages
    let node_th1 = node.clone();
    thread::spawn(move || {
        //Helper for computing 64bits timestamp
        fn get_timestamp() -> u64 {
            let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
            let timestamp =
                timestamp.as_secs() * 1_000 + timestamp.subsec_nanos() as u64 / 1_000_000;
            timestamp
        }

        let mut next_clean_up = get_timestamp();
        loop {
            let timestamp = get_timestamp();

            //cleanup transfert
            if timestamp > next_clean_up {
                next_clean_up = get_timestamp() + 1_000;
                let mut node = node_th1.lock().unwrap();
                node.cleanup_stale_transfers(timestamp);
            }
        }
    });

    //handle received messages
    let node_th2 = node.clone();
    thread::spawn(move || {
        loop {
            //receive new frame to parse
            if let Ok((timestamp, frame)) = rx_receive_can_frame.recv() {
                let mut node = node_th2.lock().unwrap();
                node.handle_rx_frame(frame, timestamp)
            }
        }
    });

    //transfert can packet from node to simulator or serial
    let com_th3 = com.clone();
    let node_th3 = node.clone();
    thread::spawn(move || loop {
        let mut node = node_th3.lock().unwrap();
        while let Some(frame) = node.pop_tx_queue() {
            com_th3.send_can_frame(frame);
        }
        drop(node);

        thread::sleep(time::Duration::from_millis(20));
    });

    (com, rx_send_can_frame)
}

