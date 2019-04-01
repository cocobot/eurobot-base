pub mod stype;

use std::sync::Arc;
use std::sync::Mutex;

use std::collections::HashMap;
use std::thread;
use std::time;

use std::fs::File;
use std::io::BufReader;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

use com::msg::Msg;
use com::msg::QValue;
use com::Com;
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
    com: Option<Com>,
}

impl StateManager {
    pub fn new(config: ConfigManagerInstance) -> StateManagerInstance {
        let sm = Arc::new(Mutex::new(StateManager {
            state: State::new(),
            config,
            com: None,
        }));
        sm
    }

    fn set_com_instance(&mut self, com: Com) {
        self.com = Some(com);
    }

    fn send(&self, msg: Msg) {
        let com = self.com.as_ref().unwrap();
        com.message(msg);
    }

    pub fn start(state: StateManagerInstance, com: Com) {
        thread::spawn(move || {
            let mut istate = state.lock().unwrap();
            istate.set_com_instance(com.clone());
            let cnf = istate.config.clone();
            drop(istate);
            let auto_assign_id = cnf.lock().unwrap().com.auto_assign_id;
            loop {
                {
                    let boards = &cnf.lock().unwrap().com.boards;
                    let mut istate = state.lock().unwrap();
                    let st = istate.get_state_mut();

                    let mut ass_id = None;
                    let mut ass_id_mult = false;

                    for (id, node) in st.nodes.iter_mut() {
                        if node.ui_name.is_none() {
                            for b in boards {
                                if *id == b.id {
                                    node.ui_name = Some(b.name.to_string());
                                    break;
                                }
                            }
                        }

                        if node.info_needed() {
                            com.message(Msg::GetNodeInfo { node_id: *id });
                            node.stamp_node_info();
                        }
                        node.check_offline();
                        if node.check_id(auto_assign_id) {
                            if ass_id.is_some() {
                                ass_id_mult = false;
                            } else {
                                ass_id = Some(node);
                            }
                        }
                    }

                    if ass_id_mult {
                        error!("Multiple board found with id 127");
                    } else if let Some(n) = ass_id {
                        if n.can_assign_id() {
                            let mut found = false;
                            for b in boards {
                                if &b.uid == n.uid.as_ref().unwrap() {
                                    found = true;
                                    com.message(Msg::Set {
                                        node_id: n.id,
                                        name: "ID".to_string(),
                                        value: QValue::I64(b.id as i64),
                                    });
                                    n.set_assigned();
                                    info!("Set new board id to {}", b.id);
                                    break;
                                }
                            }
                            if !found {
                                warn!("No board definition for {}", n.uid.as_ref().unwrap());
                            }
                        }
                    }

                    drop(st);
                    drop(istate);
                    drop(boards);
                }

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
        let node = self
            .state
            .nodes
            .entry(node_id)
            .or_insert(stype::NodeInfo::new(node_id));
        node.stamp();
        node.uptime_sec = Some(status.uptime_sec);
        node.health = stype::NodeInfoHealth::from_u8(status.health);
        node.mode = stype::NodeInfoMode::from_u8(status.mode);
    }

    pub fn set_node_info(
        &mut self,
        node_id: u8,
        info: dsdl::uavcan::protocol::GetNodeInfoResponse,
    ) {
        let node = self
            .state
            .nodes
            .entry(node_id)
            .or_insert(stype::NodeInfo::new(node_id));
        node.stamp();
        node.uptime_sec = Some(info.status.uptime_sec);
        node.health = stype::NodeInfoHealth::from_u8(info.status.health);
        node.mode = stype::NodeInfoMode::from_u8(info.status.mode);
        node.soft_version = Some(format!(
            "{}.{}",
            info.software_version.major, info.software_version.minor
        ));
        node.git = Some(info.software_version.vcs_commit);
        node.hard_version = Some(format!(
            "{}.{}",
            info.hardware_version.major, info.hardware_version.minor
        ));
        node.uid = Some(format!("{:02X}", info.hardware_version.unique_id.as_hex()));

        if let Ok(name) = std::str::from_utf8(&info.name) {
            node.name = Some(name.to_string());
        }
    }

    pub fn command(&self, cmd: &str) {
        let split: Vec<&str> = cmd.split(' ').collect();
        debug!("plsit: {:?}", split);
        if split.len() > 0 {
            match split[0] {
                "pgm" => {
                    if split.len() > 1 {
                        let id = split[1].to_string().parse::<u8>();
                        match id {
                            Ok(id) => {
                                self.send(Msg::Program { node_id: id });
                            }
                            _ => {
                                warn!("bad pgm format '{}'", cmd);
                            }
                        }
                    } else {
                        warn!("bad pgm format '{}'", cmd);
                    }
                }
                "restart" => {
                    if split.len() > 1 {
                        let id = split[1].to_string().parse::<u8>();
                        match id {
                            Ok(id) => {
                                self.send(Msg::Restart { node_id: id });
                            }
                            _ => {
                                warn!("bad pgm format '{}'", cmd);
                            }
                        }
                    } else {
                        warn!("bad pgm format '{}'", cmd);
                    }
                }
                _ => {
                    warn!("bad command '{}'", cmd);
                }
            }
        }
    }

    fn send_read_data(&self, node_id: u8, path: &str, offset: u64) -> std::io::Result<()> {
        let f = File::open(path)?;
        let mut reader = BufReader::new(f);
        reader.seek(SeekFrom::Start(offset))?;

        let mut buffer = [0; 256];
        let count = reader.read(&mut buffer)?;
        self.send(Msg::ReadResponse {
            node_id,
            error: false,
            data: buffer[0..count].to_vec(),
        });

        Ok(())
    }

    pub fn request_read(&mut self, node_id: u8, read: dsdl::uavcan::protocol::file::ReadRequest) {
        info!("FIRMWARE UPDATE: {} -> {:?}", node_id, read);
        let boards = &self.config.lock().unwrap().com.boards;
        info!("UNLOCK");
        for b in boards {
            debug!("{} {}", node_id, b.id);
            if node_id == b.id {
                match &b.path {
                    Some(path) => {
                        warn!("Board {:?}  -> {:?}", node_id, path);
                        if self.send_read_data(node_id, path, read.offset).is_err() {
                            self.send(Msg::ReadResponse {
                                node_id,
                                error: true,
                                data: Vec::new(),
                            });
                        }
                    }
                    None => {
                        error!("Board {:?} has no loadable firmware", node_id);
                    }
                };
                break;
            }
        }
    }
}
