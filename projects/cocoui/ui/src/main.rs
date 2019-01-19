#[macro_use]
extern crate lazy_static;
extern crate state_manager;

mod ui;

fn main() {
    let state = state_manager::init(42);

    ui::init(state);
    ui::start();
}
