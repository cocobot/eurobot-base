extern crate canars;
extern crate dsdl;

mod serial;
pub mod msg;

use std::sync::Arc;
use std::sync::Mutex;

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
    fn should_accept_transfer(&self, _com: &ComInstance, data_type_signature: &mut u64, data_type_id: u16, _transfer_type : TransferType, _source_node_id: u8) -> bool {
        if dsdl::uavcan::protocol::NodeStatus::check_id(data_type_id) {
            dsdl::uavcan::protocol::NodeStatus::set_signature(data_type_signature);
            true
        }
        else {
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
    }
}

pub struct Com {
    node: Option<Arc<Mutex<Instance<ComHandler, ComInstance>>>>,
    state_manager: StateManagerInstance,
}

impl Com {
    fn new(state_manager: StateManagerInstance) -> Com {
         Com {
             node: None,
             state_manager,
         }
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
        println!("Todo: {:?}", msg);
    }
}

pub fn init(node_id: u8, state_manager: StateManagerInstance) {
    let com = Arc::new(Mutex::new(Com::new(state_manager)));
    let node = Instance::init(ComHandler::new(), com.clone());

    //acquire lock for configuration
    let com_cpy = com.clone();
    let mut mcom = com.lock().unwrap();
    mcom.set_node(node);
    StateManager::start(mcom.get_state_manager_mut().clone(), com_cpy);
    drop(mcom);

    serial::init(com.clone());
}
