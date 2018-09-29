#[macro_use]
extern crate futures;

mod network;
mod protocol;
mod state;
mod robot;

use std::sync::{Arc, Mutex};
use state::State;

fn main() {
    let state = Arc::new(Mutex::new(State::new()));

    network::init(state.clone());
}
