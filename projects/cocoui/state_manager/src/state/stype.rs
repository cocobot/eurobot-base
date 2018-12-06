use std::time::SystemTime;
use std::time::Duration;

#[derive(Debug, Copy, Clone)]
pub enum NodeInfoHealth {
    Ok,
    Warning,
    Error,
    Critical,
}

impl NodeInfoHealth {
     pub fn from_u8(value: u8) -> Option<NodeInfoHealth> {
         match value {
             dsdl::uavcan::protocol::NodeStatus::HEALTH_OK => Some(NodeInfoHealth::Ok),
             dsdl::uavcan::protocol::NodeStatus::HEALTH_WARNING => Some(NodeInfoHealth::Warning),
             dsdl::uavcan::protocol::NodeStatus::HEALTH_ERROR => Some(NodeInfoHealth::Error),
             dsdl::uavcan::protocol::NodeStatus::HEALTH_CRITICAL => Some(NodeInfoHealth::Critical),
             _ => None,
         }
     }
}

#[derive(Debug, Copy, Clone)]
pub enum NodeInfoMode {
    Operational,
    Initialization,
    Maintenance,
    SoftwareUpdate,
    Offline,
}

impl NodeInfoMode {
     pub fn from_u8(value: u8) -> Option<NodeInfoMode> {
         match value {
             dsdl::uavcan::protocol::NodeStatus::MODE_OPERATIONAL => Some(NodeInfoMode::Operational),
             dsdl::uavcan::protocol::NodeStatus::MODE_INITIALIZATION => Some(NodeInfoMode::Initialization),
             dsdl::uavcan::protocol::NodeStatus::MODE_MAINTENANCE => Some(NodeInfoMode::Maintenance),
             dsdl::uavcan::protocol::NodeStatus::MODE_SOFTWARE_UPDATE => Some(NodeInfoMode::SoftwareUpdate),
             dsdl::uavcan::protocol::NodeStatus::MODE_OFFLINE => Some(NodeInfoMode::Offline),
             _ => None,
         }
     }
}

#[derive(Debug, Copy, Clone)]
pub struct NodeInfo {
    pub id: u8,
    pub uptime_sec: Option<u32>,
    pub health: Option<NodeInfoHealth>,
    pub mode: Option<NodeInfoMode>,

    last_stamp : SystemTime,
    last_node_info : SystemTime,
}

impl NodeInfo {
    pub fn new(id: u8) -> NodeInfo {
        NodeInfo {
            id,
            uptime_sec: None,
            health: None,
            mode: None,
            last_stamp: SystemTime::now(),
            last_node_info: SystemTime::UNIX_EPOCH,
        }
    }

    pub fn stamp(&mut self) {
        self.last_stamp = SystemTime::now();
    }

    pub fn stamp_node_info(&mut self) {
        self.last_node_info = SystemTime::now();
    }

    pub fn info_needed(&self) -> bool {
        self.last_node_info.elapsed().unwrap() > Duration::from_secs(10)
    }
}

