extern crate dsdl;

pub mod msg;
pub mod serial;

use crossbeam_channel::unbounded;
use crossbeam_channel::{Receiver, Sender};
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time;
use std::time::Duration;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

use crate::state::StateManagerInstance;
use canars::CANFrame;
use canars::Instance;
use canars::Node;
use canars::RxTransfer;
use canars::TransferType;
use msg::Msg;

pub struct ComHandler {}

impl ComHandler {
    fn new() -> ComHandler {
        ComHandler {}
    }
}

impl Node<StateManagerInstance> for ComHandler {
    fn should_accept_transfer(
        &self,
        _state: &StateManagerInstance,
        data_type_signature: &mut u64,
        data_type_id: u16,
        _transfer_type: TransferType,
        source_node_id: u8,
    ) -> bool {
        //Indicate canars if trame is useful or not
        if dsdl::uavcan::protocol::NodeStatus::check_id(data_type_id) {
            dsdl::uavcan::protocol::NodeStatus::set_signature(data_type_signature);
            true
        } else if dsdl::uavcan::protocol::GetNodeInfoResponse::check_id(data_type_id) {
            dsdl::uavcan::protocol::GetNodeInfoResponse::set_signature(data_type_signature);
            true
        } else if dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::check_id(data_type_id)
        {
            dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::set_signature(
                data_type_signature,
            );
            true
        } else if dsdl::uavcan::protocol::file::ReadRequest::check_id(data_type_id) {
            dsdl::uavcan::protocol::file::ReadRequest::set_signature(data_type_signature);
            true
        } 
        else if dsdl::uavcan::cocobot::Position::check_id(data_type_id) {
            dsdl::uavcan::cocobot::Position::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::cocobot::GameState::check_id(data_type_id) {
            dsdl::uavcan::cocobot::GameState::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::protocol::debug::LogMessage::check_id(data_type_id) {
            dsdl::uavcan::protocol::debug::LogMessage::set_signature(data_type_signature);
            true
        } else {
            debug!("xfer refused: {}", source_node_id);
            false
        }
    }

    fn on_transfer_reception(&self, state: &mut StateManagerInstance, xfer: &RxTransfer) {
        let mut state_manager = state.lock().unwrap();

        //Route packet depeding on id
        if dsdl::uavcan::protocol::NodeStatus::check_id(xfer.get_data_type_id()) {
            let status = dsdl::uavcan::protocol::NodeStatus::decode(xfer).unwrap();
            state_manager.set_node_status(xfer.get_source_node_id(), status);
        } else if dsdl::uavcan::protocol::GetNodeInfoResponse::check_id(xfer.get_data_type_id()) {
            match dsdl::uavcan::protocol::GetNodeInfoResponse::decode(xfer) {
              Some(value) => {state_manager.set_node_info(xfer.get_source_node_id(), value);
              }
              _ => {},
            };
        } else if dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::check_id(
            xfer.get_data_type_id(),
        ) {
            let update =
                dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::decode(xfer).unwrap();
            info!("FIRMWARE UPDATE: {:?}", update);
        } else if dsdl::uavcan::protocol::file::ReadRequest::check_id(xfer.get_data_type_id()) {
            let read = dsdl::uavcan::protocol::file::ReadRequest::decode(xfer).unwrap();
            state_manager.request_read(xfer.get_source_node_id(), read);
       } else if dsdl::uavcan::protocol::debug::LogMessage::check_id(xfer.get_data_type_id()) {
            let msg = dsdl::uavcan::protocol::debug::LogMessage::decode(xfer).unwrap();

            if let Ok(name) = std::str::from_utf8(&msg.text) {
                match msg.level.value {
                    dsdl::uavcan::protocol::debug::LogLevel::DEBUG => {
                        debug!("{:?}", name.to_string());
                    },
                    dsdl::uavcan::protocol::debug::LogLevel::INFO => {
                        info!("{:?}", name.to_string());
                    },
                    dsdl::uavcan::protocol::debug::LogLevel::WARNING => {
                        warn!("{:?}", name.to_string());
                    },
                    dsdl::uavcan::protocol::debug::LogLevel::ERROR => {
                        error!("{:?}", name.to_string());
                    },

                    _ => {},
                }
            }
        } else if dsdl::uavcan::cocobot::Position::check_id(xfer.get_data_type_id()) {
            let position = dsdl::uavcan::cocobot::Position::decode(xfer).unwrap();

            let state = state_manager.get_state_mut();
            let id = if xfer.get_source_node_id() < 30 { 0 } else { 1 };
            state.robots[id].x = position.x.into();
            state.robots[id].y = position.y.into();
            state.robots[id].a = position.a.into();
        } else if dsdl::uavcan::cocobot::GameState::check_id(xfer.get_data_type_id()) {
            let position = dsdl::uavcan::cocobot::GameState::decode(xfer).unwrap();

            let state = state_manager.get_state_mut();
            let id = if xfer.get_source_node_id() < 30 { 0 } else { 1 };
            state.robots[id].battery = position.battery.into();
            state.robots[id].time = position.time.into();
            state.robots[id].score = position.score.into();
            state.robots[id].color = position.color.into();
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
    send_msg: Sender<msg::Msg>,
}

impl Com {
    fn new(
        receive_can_frame: Sender<(u64, CANFrame)>,
        send_can_frame: Sender<CANFrame>,
        send_msg: Sender<msg::Msg>,
    ) -> Com {
        //Allocation for com structure
        Com {
            receive_can_frame,
            send_can_frame,
            send_msg,
        }
    }

    pub fn send_can_frame(&self, frame: CANFrame) {
        info!("TX SEND {:?}", frame);
        match self.send_can_frame.send(frame) {
            Ok(_) => {
                info!("OK !");
            }
            Err(e) => {
                error!("send_can_frame: {:?}", e);
            }
        }
    }

    pub fn message(&self, msg: Msg) {
        match self.send_msg.send(msg) {
            Ok(_) => {}
            Err(e) => {
                error!("send_msg: {:?}", e);
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

pub fn init(node_id: u8, state_manager: StateManagerInstance) -> (Com, Receiver<CANFrame>) {
    //Canars node initialisation
    let mut node = Instance::init(ComHandler::new(), state_manager.clone());
    node.set_local_node_id(node_id);

    //protect node with mutex
    let node = Arc::new(Mutex::new(node));

    //create channels for communication
    let (tx_send_msg, rx_send_msg) = unbounded();
    let (tx_send_can_frame, rx_send_can_frame) = unbounded();
    let (tx_receive_can_frame, rx_receive_can_frame) = unbounded();

    //Com initialisation
    let com = Com::new(tx_receive_can_frame, tx_send_can_frame, tx_send_msg);

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

            //compute max waiting time before next cleanup
            let delta = if next_clean_up > timestamp {
                next_clean_up - timestamp
            } else {
                0
            };

            //receive new message to send
            if let Ok(msg) = rx_send_msg.recv_timeout(Duration::from_millis(delta)) {
                let mut node = node_th1.lock().unwrap();
                msg.send(&mut node);
            }

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
