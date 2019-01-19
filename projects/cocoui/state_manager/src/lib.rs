#![feature(nll)]

mod com;
pub mod state;

pub fn init(node_id: u8) -> state::StateManagerInstance {
    let state_manager = state::StateManager::new();

    com::init(node_id, state_manager.clone());

    state_manager
}
