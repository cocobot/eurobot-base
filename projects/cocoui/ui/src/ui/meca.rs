use crate::ui::gtk::prelude::*;
use std::cell::RefCell;

use state_manager::state::StateManagerInstance;

thread_local!(
    static PMECA: RefCell<MecaWindow> = RefCell::new(MecaWindow::new());
    static SMECA: RefCell<MecaWindow> = RefCell::new(MecaWindow::new());
);

struct MecaWindow {
    window: Option<gtk::Window>,
    state: Option<StateManagerInstance>,
    principal: bool,
}

impl MecaWindow {
    fn new() -> MecaWindow {
        MecaWindow {
            window: None,
            state: None,
            principal: true,
        }
    }

    fn init(&mut self, state: StateManagerInstance, principal: bool) {
        self.state = Some(state);
        self.principal = principal;
    }

    fn set_servo(&self, id: usize, value: &str, deg: bool) {
      if let Some(state) = &self.state {
        let mode = if deg { 1 } else { 0 };
        let cmd = format!("servo 15 {} {} {}", mode, id, value);
        state.lock().unwrap().command(&cmd);
      }
    }

    fn cmd(&self, cmd: &str) {
      if let Some(state) = &self.state {
        state.lock().unwrap().command(cmd);
      }
    }

    fn show(&mut self) {
        match self.window {
            Some(ref mut w) => w.present(),
            None => {
                let glade_src = include_str!("glade/meca.glade");
                let builder = gtk::Builder::new_from_string(glade_src);
                let window: gtk::Window = builder.get_object("mecaWindow").unwrap();

                if self.principal {
                    window.set_title("Meca (Principal)");
                }
                else {
                    window.set_title("Meca (PMI)");
                }

                window.set_position(gtk::WindowPosition::Center);


                if self.principal {
                    window.connect_delete_event(|_, _| {
                        PMECA.with(|boards| {
                            let mut boards = boards.borrow_mut();
                            boards.delete_event();
                        });
                    Inhibit(false)
                    });
                }
                else {
                    window.connect_delete_event(|_, _| {
                        SMECA.with(|boards| {
                            let mut boards = boards.borrow_mut();
                            boards.delete_event();
                        });
                        Inhibit(false)
                    });
                }

                for i in 0..12 {
                  let btn : gtk::Button = builder.get_object(&format!("valid_s{}", i)).unwrap();
                  let entry : gtk::Entry = builder.get_object(&format!("entry_s{}", i)).unwrap();
                  let check : gtk::CheckButton = builder.get_object(&format!("servo_angle")).unwrap();
                  let check2 = check.clone();
                  let entry2 = entry.clone();
                  let entry3 = entry.clone();
                  btn.connect_clicked( move |_| {
                        PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.set_servo(i, entry.get_text().unwrap().as_str(), check.get_active());
                        });
                  });
                  entry2.connect_activate( move |_| {
                        PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.set_servo(i, entry3.get_text().unwrap().as_str(), check2.get_active());
                        });
                  });
                }

                let x : gtk::Entry = builder.get_object("entry_x").unwrap();
                let y : gtk::Entry = builder.get_object("entry_y").unwrap();
                let z : gtk::Entry = builder.get_object("entry_z").unwrap();
                let d : gtk::Entry = builder.get_object("entry_d").unwrap();
                let a : gtk::Entry = builder.get_object("entry_a").unwrap();
                let arm : gtk::Entry = builder.get_object("entry_arm").unwrap();
                {
                  let x = x.clone();
                  let y = y.clone();
                  let z = z.clone();
                  let a = a.clone();
                  let arm = arm.clone();
                  let btn : gtk::Button = builder.get_object("btn_arm").unwrap();
                  btn.connect_clicked( move |_| {
                    PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("meca 15 12 {} {} {} {} {} {}",
                            arm.get_text().unwrap().as_str(), 
                            x.get_text().unwrap().as_str(), 
                            y.get_text().unwrap().as_str(), 
                            z.get_text().unwrap().as_str(), 0, 
                            a.get_text().unwrap().as_str()
                          ));
                     });
                  });
                }
                {
                  let arm = arm.clone();
                  let btn : gtk::Button = builder.get_object("btn_prendre_sol").unwrap();
                  btn.connect_clicked( move |_| {
                    PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("meca 15 5 {} 0 0 0 0 0",
                            arm.get_text().unwrap().as_str() 
                          ));
                     });
                  });
                }
                {
                  let arm = arm.clone();
                  let btn : gtk::Button = builder.get_object("btn_repos_normal").unwrap();
                  btn.connect_clicked( move |_| {
                    PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("meca 15 7 {} 0 0 0 0 0",
                            arm.get_text().unwrap().as_str()
                          ));
                     });
                  });
                }
                {
                  let arm = arm.clone();
                  let btn : gtk::Button = builder.get_object("btn_repos_vide").unwrap();
                  btn.connect_clicked( move |_| {
                    PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("meca 15 6 {} 0 0 0 0 0",
                            arm.get_text().unwrap().as_str()
                          ));
                     });
                  });
                }
                {
                  let btn : gtk::Button = builder.get_object("btn_init").unwrap();
                  btn.connect_clicked( move |_| {
                    PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("meca 15 1 0 0 0 0 0 0"
                          ));
                     });
                  });
                }

                for i in 0..4 {
                  let btn : gtk::Button = builder.get_object(&format!("btn_pompe_on_{}", i)).unwrap();
                  btn.connect_clicked( move |_| {
                        PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("pump 15 {} 1", i));
                        });
                  });

                  let btn : gtk::Button = builder.get_object(&format!("btn_pompe_off_{}", i)).unwrap();
                  btn.connect_clicked( move |_| {
                        PMECA.with(|meca| {
                          let meca = meca.borrow();
                          meca.cmd(&format!("pump 15 {} 0", i));
                        });
                  });
                }

                window.show_all();
                self.window = Some(window);
            }
        }
    }

    fn delete_event(&mut self) {
        self.window = None;
    }
}

pub fn show(principal: bool) {
    if principal {
        PMECA.with(|m| {
            let mut m = m.borrow_mut();
            m.show();
        });
    }
    else {
        SMECA.with(|m| {
            let mut m = m.borrow_mut();
            m.show();
        });
    }
}

pub fn init(state: StateManagerInstance) {
    let pstate = state.clone();
    PMECA.with(|m| {
        let mut m = m.borrow_mut();
        m.init(pstate, true);
    });

    let sstate = state.clone();
    SMECA.with(|m| {
        let mut m = m.borrow_mut();
        m.init(sstate, false);
    });
}


