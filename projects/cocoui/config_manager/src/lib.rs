#![feature(nll)]

extern crate toml;
extern crate serde;
#[macro_use]
extern crate serde_derive;

pub mod config;
mod defs;

pub fn init() -> config::ConfigManagerInstance {
    let config_manager = config::ConfigManager::new();

    config_manager
}
