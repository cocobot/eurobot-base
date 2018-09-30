#[macro_use]
extern crate futures;

mod ui;
mod network;
mod protocol;
mod robot;

use std::sync::mpsc::{Sender, Receiver};
use std::sync::mpsc;

use robot::RobotData;

fn main() {
    let (tx_rdata, rx_rdata): (Sender<RobotData>, Receiver<RobotData>) = mpsc::channel();

    network::init(&tx_rdata);

    ui::init(rx_rdata);
    ui::start();
}
