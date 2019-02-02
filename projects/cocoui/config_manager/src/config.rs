use std::sync::Arc;
use std::sync::Mutex;
use defs;

pub type ConfigManagerInstance = Arc<Mutex<ConfigManager>>;

pub struct ConfigManager {
    pub com: defs::com::Config,
    pub eurobot: defs::eurobot::Config,
}

impl ConfigManager {
    pub fn new() -> ConfigManagerInstance {
        let cm = Arc::new(Mutex::new(ConfigManager {
            com: defs::com::Config::new("com"),
            eurobot: defs::eurobot::Config::new("eurobot"),
        }));
        cm
    }
}
