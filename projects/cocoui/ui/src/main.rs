#[macro_use]
extern crate futures;
#[macro_use]
extern crate lazy_static;

mod ui;
mod network;
mod serial;
mod protocol;
mod com;
mod robot;

use std::sync::mpsc::{Sender, Receiver};
use std::sync::mpsc;

use robot::RobotData;

fn main() {
    let (tx_rdata, rx_rdata): (Sender<RobotData>, Receiver<RobotData>) = mpsc::channel();

    let ith = com::init();
    //network::init(&tx_rdata);
    serial::init(ith);

    ui::init(rx_rdata);
    ui::start();
}
