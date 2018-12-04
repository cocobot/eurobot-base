extern crate canars;
extern crate dsdl;

mod serial;

use std::sync::Arc;
use std::sync::Mutex;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;

use com::canars::Instance;
use com::canars::TransferType;
use com::canars::RxTransfer;
use com::canars::Node;
use com::canars::CANFrame;

type ComInstance = Arc<Mutex<Com>>;

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

    fn on_transfer_reception(&self, _com: &mut ComInstance, xfer: &RxTransfer) {
        if dsdl::uavcan::protocol::NodeStatus::check_id(xfer.get_data_type_id()) {
            let status = dsdl::uavcan::protocol::NodeStatus::decode(xfer);
            println!("OTC: {:?}", status);
        }
    }
}

pub struct Com {
    node: Option<Instance<ComHandler, ComInstance>>,
}

impl Com {
    fn new() -> Com {
         Com {
             node: None,
         }
    }

    fn set_node(&mut self, node: Instance<ComHandler, ComInstance>) {
        self.node = Some(node);
    }

    fn handle_rx_frame(&mut self, frame: CANFrame) {
        let timestamp =  SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
        let timestamp = timestamp.as_secs() * 1_000 +
            timestamp.subsec_nanos() as u64 / 1_000_000;

        match self.node {
            Some(ref mut n) =>  n.handle_rx_frame(frame, timestamp),
            None => (),
        }

    }
}

pub fn init(node_id: u8) {
    let com = Arc::new(Mutex::new(Com::new()));
    let node = Instance::init(ComHandler::new(), com.clone());

    //acquire lock for configuration
    let mut mcom = com.lock().unwrap();
    mcom.set_node(node);
    drop(mcom);

    serial::init(com.clone());
}
