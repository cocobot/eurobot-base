extern crate state_manager;
extern crate config_manager;
extern crate pretty_env_logger;
#[macro_use]
extern crate log;

mod ui;

fn main() {
    pretty_env_logger::formatted_builder().filter_level(log::LevelFilter::Debug).init();

    info!("CocoUI !");

    let config = config_manager::init();
    info!("Configuration loaded");

    let state = state_manager::init(config.clone());
    info!("State module ready");

    ui::init(config.clone(), state);
    info!("UI initialized");
    ui::start();
}
