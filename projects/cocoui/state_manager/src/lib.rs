#![feature(nll)]
#[macro_use]
extern crate log;
extern crate config_manager;
extern crate hex_slice;

mod com;
mod simu;
pub mod state;

use config_manager::config::ConfigManagerInstance;

pub fn init(config: ConfigManagerInstance) -> state::StateManagerInstance {
    let state_manager = state::StateManager::new(config.clone());

    let conf = config.lock().unwrap();
    let simulation = conf.simulation;
    com::init(conf.com.node_id, state_manager.clone(), conf.simulation);
    drop(conf);
    

    if simulation {
        simu::init(config.clone());
    }

    state_manager
}
