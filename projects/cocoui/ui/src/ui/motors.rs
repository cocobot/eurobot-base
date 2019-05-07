use crate::ui::gtk::prelude::*;
use std::cell::RefCell;

use state_manager::state::StateManagerInstance;

thread_local!(
    static PMOTORS: RefCell<MotorsWindow> = RefCell::new(MotorsWindow::new());
    static SMOTORS: RefCell<MotorsWindow> = RefCell::new(MotorsWindow::new());
);

struct MotorsWindow {
    window: Option<gtk::Window>,
    state: Option<StateManagerInstance>,
    principal: bool,
}

impl MotorsWindow {
    fn new() -> MotorsWindow {
        MotorsWindow {
            window: None,
            state: None,
            principal: true,
        }
    }

    fn init(&mut self, state: StateManagerInstance, principal: bool) {
        self.state = Some(state);
        self.principal = principal;
    }

    fn show(&mut self) {
        match self.window {
            Some(ref mut w) => w.present(),
            None => {
                let glade_src = include_str!("glade/ui.glade");
                let builder = gtk::Builder::new_from_string(glade_src);
                let window: gtk::Window = builder.get_object("motorWindow").unwrap();

                window.show_all();

                if self.principal {
                    window.connect_delete_event(|_, _| {
                        PMOTORS.with(|boards| {
                            let mut boards = boards.borrow_mut();
                            boards.delete_event();
                        });
                    Inhibit(false)
                    });
                }
                else {
                    window.connect_delete_event(|_, _| {
                        SMOTORS.with(|boards| {
                            let mut boards = boards.borrow_mut();
                            boards.delete_event();
                        });
                        Inhibit(false)
                    });

                }
            }
        }
    }

    fn delete_event(&mut self) {
        self.window = None;
    }
}

pub fn show(principal: bool) {
    if principal {
        PMOTORS.with(|m| {
            let mut m = m.borrow_mut();
            m.show();
        });
    }
    else {
        SMOTORS.with(|m| {
            let mut m = m.borrow_mut();
            m.show();
        });
    }
}

pub fn init(state: StateManagerInstance) {
    let pstate = state.clone();
    PMOTORS.with(|m| {
        let mut m = m.borrow_mut();
        m.init(pstate, true);
    });

    let sstate = state.clone();
    SMOTORS.with(|m| {
        let mut m = m.borrow_mut();
        m.init(sstate, false);
    });
}

