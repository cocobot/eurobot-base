extern crate canars;
extern crate dsdl;

mod serial;
pub mod msg;

use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;
use std::sync::mpsc::{Sender, Receiver};
use std::sync::mpsc;
use std::time::Duration;

use com::canars::Instance;
use com::canars::TransferType;
use com::canars::RxTransfer;
use com::canars::Node;

use super::state::StateManager;
use super::state::StateManagerInstance;

pub type ComInstance = Arc<Mutex<Com>>;

struct ComHandler {
}

impl ComHandler {
    fn new() -> ComHandler {
         ComHandler { }
    }
}

impl Node<ComInstance> for ComHandler {
    fn should_accept_transfer(&self, _com: &ComInstance, data_type_signature: &mut u64, data_type_id: u16, _transfer_type : TransferType, source_node_id: u8) -> bool {
        if dsdl::uavcan::protocol::NodeStatus::check_id(data_type_id) {
            dsdl::uavcan::protocol::NodeStatus::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::protocol::GetNodeInfoResponse::check_id(data_type_id) {
            dsdl::uavcan::protocol::GetNodeInfoResponse::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::check_id(data_type_id) {
            dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::set_signature(data_type_signature);
            true
        }
        else if dsdl::uavcan::protocol::file::ReadRequest::check_id(data_type_id) {
            dsdl::uavcan::protocol::file::ReadRequest::set_signature(data_type_signature);
            true
        }
        else {
            debug!("xfer refused: {}", source_node_id);
            false
        }
    }

    fn on_transfer_reception(&self, com: &mut ComInstance, xfer: &RxTransfer) {
        let mut com = com.lock().unwrap();
        let mut state_manager = com.get_state_manager_mut().lock().unwrap();

        if dsdl::uavcan::protocol::NodeStatus::check_id(xfer.get_data_type_id()) {
            let status = dsdl::uavcan::protocol::NodeStatus::decode(xfer).unwrap();
            state_manager.set_node_status(xfer.get_source_node_id(), status);
        }
        else if dsdl::uavcan::protocol::GetNodeInfoResponse::check_id(xfer.get_data_type_id()) {
            let node_info = dsdl::uavcan::protocol::GetNodeInfoResponse::decode(xfer).unwrap();
            state_manager.set_node_info(xfer.get_source_node_id(), node_info);
        }
        else if dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::check_id(xfer.get_data_type_id()) {
            let update = dsdl::uavcan::protocol::file::BeginFirmwareUpdateResponse::decode(xfer).unwrap();
            info!("FIRMWARE UPDATE: {:?}", update);
        }
        else if dsdl::uavcan::protocol::file::ReadRequest::check_id(xfer.get_data_type_id()) {
            let read = dsdl::uavcan::protocol::file::ReadRequest::decode(xfer).unwrap();
            state_manager.request_read(xfer.get_source_node_id(), read);
        }
        else {
            error!("Xfer accepted but not implemented: {:?}", xfer.get_data_type_id());
        }
    }
}

pub struct Com {
    node: Option<Arc<Mutex<Instance<ComHandler, ComInstance>>>>,
    state_manager: StateManagerInstance,
    tx: Sender<msg::Msg>,
}

impl Com {
    fn new(state_manager: StateManagerInstance, tx: Sender<msg::Msg>) -> Com {
         Com {
             node: None,
             state_manager,
             tx,
         }
    }

    pub fn get_tx(&self) -> Sender<msg::Msg> {
         self.tx.clone()
    }

    fn set_node(&mut self, node: Instance<ComHandler, ComInstance>) {
        self.node = Some(Arc::new(Mutex::new(node)));
    }

    fn get_node(&mut self) -> Option<Arc<Mutex<Instance<ComHandler, ComInstance>>>> {
        match self.node {
            Some(ref mut s) => Some(s.clone()),
            None => None,
        }
    }

    fn get_state_manager_mut(&mut self) -> &mut StateManagerInstance {
         &mut self.state_manager
    }

    pub fn message(&mut self, msg: msg::Msg) {
        if let Err(e) = self.tx.send(msg) {
            warn!("Tx send error: {:?}", e.to_string());
        }
    }
}

pub fn init(node_id: u8, state_manager: StateManagerInstance, simulation: bool) {
    //create channel for sending Msg only in one thread
    let (tx, rx): (Sender<msg::Msg>, Receiver<msg::Msg>) = mpsc::channel();

    let com = Arc::new(Mutex::new(Com::new(state_manager, tx)));
    let mut node = Instance::init(ComHandler::new(), com.clone());
    node.set_local_node_id(node_id);

    //acquire lock for configuration
    let com_cpy = com.clone();
    let mut mcom = com.lock().unwrap();
    mcom.set_node(node);
    StateManager::start(mcom.get_state_manager_mut().clone(), com_cpy);
    drop(mcom);

    if !simulation {
        serial::init(com.clone());
    }

    thread::spawn(move || {
        fn get_timestamp() -> u64 {
            let timestamp =  SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
            let timestamp = timestamp.as_secs() * 1_000 +
                timestamp.subsec_nanos() as u64 / 1_000_000;
            timestamp
        }

        let com = com.clone();
        let mut next_clean_up = get_timestamp();
        loop {
            let timestamp = get_timestamp();

            let delta = if next_clean_up > timestamp {
                next_clean_up - timestamp
            }
            else {
                0
            };
            if let Ok(msg) = rx.recv_timeout(Duration::from_millis(delta)) {
                let mut com = com.as_ref().lock().unwrap();
                msg.send(&mut com);
            }

            if timestamp > next_clean_up {
                next_clean_up = get_timestamp() + 1_000;

                let mut com_locked = com.as_ref().lock().unwrap();
                match com_locked.get_node() {
                    Some(n) => {
                        let mut node = n.lock().unwrap();
                        node.cleanup_stale_transfers(timestamp);
                    },
                    _ => {}
                }
            }
        }
    });
}
