#[macro_use]
extern crate log;
extern crate canars;
extern crate config_manager;
extern crate crossbeam_channel;
extern crate hex_slice;

mod com;
mod simu;
pub mod state;

use config_manager::config::ConfigManagerInstance;

pub fn init(config: ConfigManagerInstance) -> state::StateManagerInstance {
    //create state manager
    let state_manager = state::StateManager::new(config.clone());

    //initialisation comunication
    let conf = config.lock().unwrap();
    let simulation = conf.simulation;
    let (com, rx_send_can_frame) = com::init(conf.com.node_id, state_manager.clone());
    drop(conf);

    //start state manager
    state::StateManager::start(state_manager.clone(), com.clone());

    //init simulation or serial link
    if simulation {
        simu::init(
            config.clone(),
            com.clone(),
            state_manager.clone(),
            rx_send_can_frame,
        );
    } else {
        //TODO: serial
        com::serial::init(com.clone(), rx_send_can_frame);
    }

    state_manager
}
