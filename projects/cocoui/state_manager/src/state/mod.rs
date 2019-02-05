pub mod stype;

use std::sync::Arc;
use std::sync::Mutex;

use std::thread;
use std::time;
use std::collections::HashMap;

use com::msg::Msg;
use com::msg::QValue;
use config_manager::config::ConfigManagerInstance;

use hex_slice::AsHex;

#[derive(Debug, Clone)]
pub struct State {
    pub nodes: HashMap<u8, stype::NodeInfo>,
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
    config: ConfigManagerInstance,
}

impl StateManager {
    pub fn new(config: ConfigManagerInstance) -> StateManagerInstance {
        let sm = Arc::new(Mutex::new(StateManager {
            state: State::new(),
            config,
        }));
        sm
    }

    pub fn start(state: StateManagerInstance, com: super::com::ComInstance) {
        thread::spawn(move || {
            let istate = state.lock().unwrap();
            let cnf = istate.config.clone();
            drop(istate);
            let auto_assign_id = cnf.lock().unwrap().com.auto_assign_id;
            let boards = &cnf.lock().unwrap().com.boards;
            loop {
                let mut istate = state.lock().unwrap();
                let st = istate.get_state_mut();

                let mut ass_id = None;
                let mut ass_id_mult = false;

                for (id, node) in st.nodes.iter_mut() {
                    if node.info_needed() {
                        let mut com = com.lock().unwrap();
                        com.message(Msg::GetNodeInfo {node_id: *id});
                        node.stamp_node_info();
                    }
                    node.check_offline();
                    if node.check_id(auto_assign_id) {
                        if ass_id.is_some() {
                            ass_id_mult = false;
                        }
                        else {
                            ass_id = Some(node);
                        }
                    }
                }
                
                if ass_id_mult {
                     error!("Multiple board found with id 127");
                }
                else if let Some(n) = ass_id {
                    if n.can_assign_id() {
                        let mut found = false;
                        for b in boards {
                            if &b.uid == n.uid.as_ref().unwrap()  {
                                 found = true;
                                 let mut com = com.lock().unwrap();
                                 com.message(Msg::Set {node_id: n.id, name: "ID".to_string(), value: QValue::I64(b.id as i64)});
                                 n.set_assigned();
                                 info!("Set new board id to {}", b.id);
                                 break;
                            }
                        }
                        if !found {
                            warn!("No board definition for {}", n.uid.as_ref().unwrap());
                        }
                    }
                   if n.need_restart() {
                       let mut com = com.lock().unwrap();
                       //com.message(Msg::Restart {node_id: n.id});
                       //n.set_offline();
                       info!("Resert board {}", n.id);
                    }
                }

                drop(st);
                drop(istate);

                thread::sleep(time::Duration::from_millis(100));
            }
        });
    }

    pub fn get_state(&self) -> State {
        self.state.clone()
    }

    fn get_state_mut(&mut self) -> &mut State {
        &mut self.state
    }

    pub fn set_node_status(&mut self, node_id: u8, status: dsdl::uavcan::protocol::NodeStatus) {
        let node = self.state.nodes.entry(node_id).or_insert(stype::NodeInfo::new(node_id));
        node.stamp();
        node.uptime_sec = Some(status.uptime_sec);
        node.health = stype::NodeInfoHealth::from_u8(status.health);
        node.mode = stype::NodeInfoMode::from_u8(status.mode);
    }

    pub fn set_node_info(&mut self, node_id: u8, info: dsdl::uavcan::protocol::GetNodeInfoResponse) {
        let node = self.state.nodes.entry(node_id).or_insert(stype::NodeInfo::new(node_id));
        node.stamp();
        node.uptime_sec = Some(info.status.uptime_sec);
        node.health = stype::NodeInfoHealth::from_u8(info.status.health);
        node.mode = stype::NodeInfoMode::from_u8(info.status.mode);
        node.soft_version = Some(format!("{}.{}", info.software_version.major, info.software_version.minor));
        node.git = Some(info.software_version.vcs_commit);
        node.hard_version = Some(format!("{}.{}", info.hardware_version.major, info.hardware_version.minor));
        node.uid = Some(format!("{:02X}", info.hardware_version.unique_id.as_hex()));
        node.name = Some(std::str::from_utf8(&info.name).unwrap().to_string());
    }
}
