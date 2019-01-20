#![feature(nll)]
extern crate state_manager;

mod ui;

fn main() {
    let config = config_manager::init();
    let state = state_manager::init(config.lock().unwrap().com.node_id);

    ui::init(state);
    ui::start();
}
