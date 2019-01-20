extern crate gtk;

use state_manager::state::StateManagerInstance;
use ui::gtk::prelude::*;

mod boards;

pub fn init(state: StateManagerInstance) {
    if gtk::init().is_err() {
        println!("Failed to initialize GTK.");
        return;
    }

    boards::init(state.clone());

    let glade_src = include_str!("glade/ui.glade");
    let builder = gtk::Builder::new_from_string(glade_src);
    let window: gtk::Window = builder.get_object("mainWindow").unwrap();

    window.connect_delete_event(|_, _| {
        // Stop the main loop.
        gtk::main_quit();
        // Let the default handler destroy the window.
        Inhibit(false)
    });

    window.set_default_size(1200, 600);

    window.show_all();
    boards::show();
}

pub fn start() {
    gtk::main();
}
