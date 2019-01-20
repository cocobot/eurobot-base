
use std::sync::Arc;
use std::sync::Mutex;
use std::fs;

#[derive(Debug, Deserialize)]
pub struct ComConfig {
    pub node_id: u8,
}

pub type ConfigManagerInstance = Arc<Mutex<ConfigManager>>;

pub struct ConfigManager {
    pub com: ComConfig,
}

impl ConfigManager {
    pub fn new() -> ConfigManagerInstance {
        let cm = Arc::new(Mutex::new(ConfigManager {
            com: toml::from_str(&fs::read_to_string("config/com.toml").unwrap()).unwrap(),
        }));
        cm
    }
}
