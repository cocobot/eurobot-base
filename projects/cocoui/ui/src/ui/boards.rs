use crate::ui::gtk::prelude::*;
use std::cell::RefCell;
use std::collections::HashMap;

use state_manager::state::StateManagerInstance;
use state_manager::state::stype::NodeInfo;
use state_manager::state::stype::NodeInfoMode;

thread_local!(
    static BOARDS: RefCell<BoardsWindow> = RefCell::new(BoardsWindow::new());
);

struct BoardsWindow {
    window: Option<gtk::Window>,
    treeview: Option<gtk::TreeView>,
    model: Option<gtk::ListStore>,
    lines: HashMap<u8, gtk::TreeIter>,
    state: Option<StateManagerInstance>,
}

impl BoardsWindow {
    fn new() -> BoardsWindow {
        BoardsWindow {
            window: None,
            treeview: None,
            model: None,
            lines: HashMap::new(),
            state: None,
        }
    }

    fn init(&mut self, state: StateManagerInstance) {
        self.state = Some(state);
    }

    fn create_column(&mut self, text: &str, col: i32, expand: bool) {
        let tvc = gtk::TreeViewColumn::new();
        tvc.set_title(text);
        let cell = gtk::CellRendererText::new();
        tvc.set_expand(expand);
        tvc.pack_start(&cell, true);
        tvc.add_attribute(&cell, "text", col);
        
        let tv = self.treeview.as_mut().unwrap();
        tv.append_column(&tvc);
        tv.show_all();
    }

    fn show(&mut self) {
        match self.window {
            Some(ref mut w) => w.present(),
            None => {
                let window = gtk::Window::new(gtk::WindowType::Toplevel);

                window.set_title("Cartes");
                window.set_default_size(400, 200);
                window.set_position(gtk::WindowPosition::Center);

                let model = gtk::ListStore::new(&[
                                                u8::static_type(),
                                                gtk::Type::String,
                                                gtk::Type::String,
                                                gtk::Type::String,
                                                gtk::Type::String,
                ]);
                model.set_sort_column_id(gtk::SortColumn::Index(1), gtk::SortType::Ascending);
                let treeview = gtk::TreeView::new_with_model(&model);
                treeview.set_hexpand(true);
                treeview.set_vexpand(true);
                treeview.set_search_column(-1);
                window.add(&treeview);

                window.show_all();

                super::create_shortcuts(window.clone());

                window.connect_delete_event(|_, _| {
                    BOARDS.with(|boards| {
                        let mut boards = boards.borrow_mut();
                        boards.delete_event();
                    });
                    Inhibit(false)
                });

                self.window = Some(window);
                self.treeview = Some(treeview);
                self.model = Some(model);

                self.create_column("id", 0, false);
                self.create_column("Nom", 1, true);
                self.create_column("Mode", 2, false);
                self.create_column("Statut", 3, false);
                self.create_column("Temps", 4, false);

                gtk::timeout_add_seconds(1, || {
                    BOARDS.with(|boards| {
                        let mut boards = boards.borrow_mut();
                        gtk::Continue(boards.update())
                    })
                });
            }
        }
    }

    fn update_row(&mut self, id: u8, node: NodeInfo) {
        let model = self.model.as_mut().unwrap();

        if !self.lines.contains_key(&id) {
            self.lines.insert(id, model.append());
        }

        let iter = self.lines.get(&id).unwrap();

        let name = format!("{} ({})", node.ui_name.unwrap_or("?".to_string()), node.name.unwrap_or("".to_string()));
        let mode = format!("{:?}", node.mode.unwrap_or(NodeInfoMode::Offline));
        let statut = match node.health {
            Some(h) => {
                format!("{:?}", h)
            },
            None => "".to_string()
        };
        let uptime = match node.uptime_sec {
            Some(sec) => {
                format!("{} s", sec)
            },
            None => "".to_string()
        };
        
        model.set(iter, &[0, 1, 2, 3, 4], &[
                  &id,
                  &name,
                  &mode,
                  &statut,
                  &uptime,
        ]);
    }

    fn delete_event(&mut self) {
        self.window = None;
    }

    fn update(&mut self) -> bool {
        match self.window {
            Some(_) => {
                let pre_state = self.state.as_ref().unwrap();
                let pre_state = pre_state.lock().unwrap();
                let state = pre_state.get_state().clone();
                drop(pre_state);

                for (key, val) in state.nodes.iter() {
                    self.update_row(*key, val.clone());
                }
                true
            },
            None => false
        }
    }
}

pub fn show() {
    BOARDS.with(|boards| {
        let mut boards = boards.borrow_mut();
        boards.show();
    });
}

pub fn init(state: StateManagerInstance) {
    BOARDS.with(|boards| {
        let mut boards = boards.borrow_mut();
        boards.init(state);
    });
}
