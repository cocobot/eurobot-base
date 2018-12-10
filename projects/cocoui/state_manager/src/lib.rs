#![feature(nll)]

mod com;
mod state;

pub fn init(node_id: u8) {
    let state_manager = state::StateManager::new();

    com::init(node_id, state_manager.clone());
}
