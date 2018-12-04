extern crate state_manager;

use std::thread;
use std::time;

fn main() {
    state_manager::init(42);

    loop {
        println!("Tick !");
        thread::sleep(time::Duration::from_millis(1000));
    }

}
