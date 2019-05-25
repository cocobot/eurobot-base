#[macro_use]
extern crate log;
extern crate pretty_env_logger;

use std::{thread, time};
use std::sync::Arc;
use std::sync::Mutex;

mod score;
mod xv11;
mod com;

pub struct ComData {
    pub score: score::Score,
    pub start: time::Instant,
    pub xv11: Arc<Mutex<xv11::XV11>>,
}

fn main() {
    pretty_env_logger::formatted_builder()
        .filter_level(log::LevelFilter::Debug)
        .init();

    info!("Lidar !");

    let mut xv11 = xv11::XV11::new();

    let data = ComData {
        score: score::Score::new(),
        start: time::Instant::now(),
        xv11: xv11.clone(),
    };

    xv11::XV11::start(&mut xv11);

    let (com, rx_send_can_frame) = com::init(14, data);
    com::serial::init(com.clone(), rx_send_can_frame);

    loop {
        thread::sleep(time::Duration::from_millis(1000));
    }
}

