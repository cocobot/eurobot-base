use std::sync::Arc;
use std::sync::Mutex;
use super::defs;

pub type ConfigManagerInstance = Arc<Mutex<ConfigManager>>;

#[derive(Debug, Clone)]
pub struct ConfigManager {
    pub simulation: bool,
    pub com: defs::com::Config,
    pub field: defs::field::Config,
    pub robots: defs::robots::Config,
}

impl ConfigManager {
    pub fn new() -> ConfigManagerInstance {
        let mut simulation = false;

        if std::env::args().nth(1).is_some() {
            simulation = true;
        }

        let cm = Arc::new(Mutex::new(ConfigManager {
            simulation,
            com: defs::com::Config::new("com"),
            field: defs::field::Config::new("field"),
            robots: defs::robots::Config::new("robots"),
        }));
        cm
    }
}
