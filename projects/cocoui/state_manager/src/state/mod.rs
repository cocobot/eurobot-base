mod stype;

use std::sync::Arc;
use std::sync::Mutex;

use std::thread;
use std::time;
use std::collections::HashMap;

use com::msg::Msg;

#[derive(Debug, Clone)]
struct State {
    nodes: HashMap<u8, stype::NodeInfo>,
}

impl State {
    fn new() -> State {
        State {
            nodes: HashMap::new(),
        }
    }
}

pub type StateManagerInstance = Arc<Mutex<StateManager>>;

pub struct StateManager {
    state: State,
}

impl StateManager {
    pub fn new() -> StateManagerInstance {
        let sm = Arc::new(Mutex::new(StateManager {
            state: State::new(),
        }));
        sm
    }

    pub fn start(state: StateManagerInstance, com: super::com::ComInstance) {
        thread::spawn(move || {
            loop {
                let istate = state.lock().unwrap();
                let st = istate.get_state();
                drop(istate);

                for (id, node) in st.nodes.iter() {
                    if node.info_needed() {
                        println!("NEED INFO : {:?}", node);
                        let mut com = com.lock().unwrap();
                        com.message(Msg::GetNodeInfo {node: *id});
                    }
                }

                thread::sleep(time::Duration::from_millis(100));
            }
        });
    }

    fn get_state(&self) -> State {
        self.state.clone()
    }

    pub fn set_node_status(&mut self, node_id: u8, status: dsdl::uavcan::protocol::NodeStatus) {
        let node = self.state.nodes.entry(node_id).or_insert(stype::NodeInfo::new(node_id));
        node.stamp();
        node.uptime_sec = Some(status.uptime_sec);
        node.health = stype::NodeInfoHealth::from_u8(status.health);
        node.mode = stype::NodeInfoMode::from_u8(status.mode);

        println!("SNS : {:?}", &node);
    }

}
