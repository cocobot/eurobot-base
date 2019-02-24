extern crate gtk;
extern crate gdk;

use std::cell::RefCell;
use state_manager::state::StateManagerInstance;
use ui::gtk::prelude::*;

mod boards;

thread_local!(
    static UII: RefCell<UI> = RefCell::new(UI::new());
);


struct UI {
    state: Option<StateManagerInstance>,
}

impl UI {
    fn new() -> UI {
        UI {
            state: None,
        }
    }
}

fn update() {
   // gtk::timeout_add(100, || {
   //     UII.with(|ui| {
   //         update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.queue_draw()});
   //         gtk::Continue(true)
   //     }
   // });
}

pub fn create_shortcuts(window: gtk::Window) {
    window.connect_key_release_event(move |_, key| {
        match key.get_keyval() {
            gdk::enums::key::P => {
                debug!("KEY P");
                UII.with(|ui| {
                    let ui = ui.borrow();

                    let state = ui.state.as_ref().unwrap().lock().unwrap();
                    state.command("pgm 11");
                });
            },
            gdk::enums::key::R => {
                debug!("KEY R");
                UII.with(|ui| {
                    let ui = ui.borrow();

                    let state = ui.state.as_ref().unwrap().lock().unwrap();
                    state.command("restart 11");
                });
            }
            _ => {},
        };

        Inhibit(false)
    });
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
    
    //init UI struct
    UII.with(|ui| {
        let mut ui = ui.borrow_mut();

        //ui.field = builder.get_object("field");
        ui.state = Some(state.clone());

        //update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.connect_draw(draw_field)});
    });
    

    create_shortcuts(window.clone());

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
