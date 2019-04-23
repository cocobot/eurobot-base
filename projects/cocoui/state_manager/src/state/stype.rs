use std::time::Duration;
use std::time::SystemTime;

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
            dsdl::uavcan::protocol::NodeStatus::MODE_INITIALIZATION => {
                Some(NodeInfoMode::Initialization)
            }
            dsdl::uavcan::protocol::NodeStatus::MODE_MAINTENANCE => Some(NodeInfoMode::Maintenance),
            dsdl::uavcan::protocol::NodeStatus::MODE_SOFTWARE_UPDATE => {
                Some(NodeInfoMode::SoftwareUpdate)
            }
            dsdl::uavcan::protocol::NodeStatus::MODE_OFFLINE => Some(NodeInfoMode::Offline),
            _ => None,
        }
    }
}

#[derive(Debug, Clone)]
pub struct NodeInfo {
    pub id: u8,
    pub uptime_sec: Option<u32>,
    pub health: Option<NodeInfoHealth>,
    pub mode: Option<NodeInfoMode>,
    pub soft_version: Option<String>,
    pub git: Option<u32>,
    pub hard_version: Option<String>,
    pub name: Option<String>,
    pub ui_name: Option<String>,
    pub uid: Option<String>,

    last_stamp: SystemTime,
    last_node_info: SystemTime,
    assigned: bool,
}

impl NodeInfo {
    pub fn new(id: u8) -> NodeInfo {
        NodeInfo {
            id,
            uptime_sec: None,
            health: None,
            mode: Some(NodeInfoMode::Offline),
            last_stamp: SystemTime::now(),
            last_node_info: SystemTime::UNIX_EPOCH,
            soft_version: None,
            git: None,
            hard_version: None,
            name: None,
            ui_name: None,
            uid: None,
            assigned: false,
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

    pub fn check_offline(&mut self) {
        if self.last_stamp.elapsed().unwrap() > Duration::from_secs(3) && self.uptime_sec.is_some()
        {
            self.set_offline();
        }
    }

    pub fn set_offline(&mut self) {
        self.uptime_sec = None;
        self.health = None;
        self.mode = Some(NodeInfoMode::Offline);
        self.assigned = false;
    }

    pub fn check_id(&mut self, auto_assign_id: bool) -> bool {
        if self.id == 127 {
            if auto_assign_id {
                true
            } else {
                warn!("Found board with id 127 but com.asign_id is false");
                false
            }
        } else {
            false
        }
    }

    pub fn can_assign_id(&self) -> bool {
        if let Some(uptime) = self.uptime_sec {
            if uptime > 5 && self.uid.is_some() && !self.assigned {
                return true;
            }
        }
        false
    }

    pub fn set_assigned(&mut self) {
        self.assigned = true;
    }
}

#[derive(Debug, Clone, Copy)]
pub struct RobotInfo {
    pub x: f64,
    pub y: f64,
    pub a: f64,

    pub simu: bool,
    pub simu_x: f64,
    pub simu_y: f64,
    pub simu_a: f64,
}

impl RobotInfo {
    pub fn new() -> RobotInfo {
        RobotInfo {
            x: 0.0,
            y: 0.0,
            a: 0.0,

            simu: false,
            simu_x: 0.0,
            simu_y: 0.0,
            simu_a: 0.0,
        }
    }
}
