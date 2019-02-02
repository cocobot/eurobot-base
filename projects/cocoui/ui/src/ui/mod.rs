extern crate gtk;

use state_manager::state::StateManagerInstance;
use ui::gtk::prelude::*;

mod boards;

struct UI {
}

fn update() {
   // gtk::timeout_add(100, || {
   //     UII.with(|ui| {
   //         update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.queue_draw()});
   //         gtk::Continue(true)
   //     }
   // });
}

pub fn init(state: StateManagerInstance) {
    if gtk::init().is_err() {
        println!("Failed to initialize GTK.");
        return;
    }

    boards::init(state.clone());

    let glade_src = include_str!("glade/ui.glade");
    let builder = gtk::Builder::new_from_string(glade_src);
    let window: gtk::Window = builder.get_object("mainWindow").unwrap();
    //
    //init UI struct
    //UII.with(|ui| {
    //    let mut ui = ui.borrow_mut();

    //    ui.field = builder.get_object("field");

    //    update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.connect_draw(draw_field)});
    //});

    window.connect_delete_event(|_, _| {
        // Stop the main loop.
        gtk::main_quit();
        // Let the default handler destroy the window.
        Inhibit(false)
    });

    window.set_default_size(1200, 600);

    window.show_all();
    update();
    boards::show();
}

pub fn start() {
    gtk::main();
}
