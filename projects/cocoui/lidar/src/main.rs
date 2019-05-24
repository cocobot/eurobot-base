#[macro_use]
extern crate log;
extern crate pretty_env_logger;

use std::{thread, time};

mod score;
mod xv11;
mod com;

pub struct ComData {
    pub score: score::Score,
    pub start: time::Instant,
}

fn main() {
    pretty_env_logger::formatted_builder()
        .filter_level(log::LevelFilter::Debug)
        .init();

    info!("Lidar !");

    let data = ComData {
        score: score::Score::new(),
        start: time::Instant::now(),
    };

    let mut xv11 = xv11::XV11::new();
    xv11::XV11::start(&mut xv11);

    let (com, rx_send_can_frame) = com::init(14, data);
    com::serial::init(com.clone(), rx_send_can_frame);

    loop {
        thread::sleep(time::Duration::from_millis(1000));
    }
}

