#![feature(nll)]
#[macro_use]
extern crate log;
extern crate config_manager;
extern crate hex_slice;

mod com;
pub mod state;

use config_manager::config::ConfigManagerInstance;

pub fn init(config: ConfigManagerInstance) -> state::StateManagerInstance {
    let state_manager = state::StateManager::new(config.clone());

    com::init(config.lock().unwrap().com.node_id, state_manager.clone());

    state_manager
}
