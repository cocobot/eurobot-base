extern crate gtk;
extern crate glib;

use std::cell::RefCell;
use std::sync::mpsc::Receiver;
use ui::gtk::prelude::*;

use robot::RobotData;

macro_rules! update_elm {
    ($a: expr, $b: expr) => {
        if $a.is_some() {
            $b($a.as_mut().unwrap())
        }
    };
}

thread_local!(
    static UII: RefCell<UI> = RefCell::new(UI::new());
);

struct UI {
    //chans
    rx_rdata: Option<Receiver<RobotData>>,

    //pr info
    pr_info_x: Option<gtk::Label>,
    pr_info_y: Option<gtk::Label>,
    pr_info_a: Option<gtk::Label>,
    pr_info_score: Option<gtk::Label>,
    pr_info_elapsed_time: Option<gtk::Label>,
    pr_info_battery: Option<gtk::Label>,
}

impl UI {
     fn new() -> UI {
          UI {
              rx_rdata: None,
              pr_info_x: None,
              pr_info_y: None,
              pr_info_a: None,
              pr_info_score: None,
              pr_info_elapsed_time: None,
              pr_info_battery: None,
          }
     }
}

pub fn update() {
    let do_update = ||-> glib::Continue {
        UII.with(|ui| {
            let mut ui = ui.borrow_mut();

            let mut pr = None;

            loop {
                match ui.rx_rdata.as_mut().unwrap().try_recv() {
                    Ok(data) => {
                        pr = Some(data);
                    },
                    _ => break,
                }
            }

            if pr.is_some() {
                let pr = pr.unwrap();
                update_elm!(ui.pr_info_x, |x: &mut gtk::Label| {x.set_text(&format!("x : {:.0} mm", pr.x_mm))});
                update_elm!(ui.pr_info_y, |x: &mut gtk::Label| {x.set_text(&format!("y : {:.0} mm", pr.y_mm))});
                update_elm!(ui.pr_info_a, |x: &mut gtk::Label| {x.set_text(&format!("a : {:.0} °", pr.a_deg))});
                update_elm!(ui.pr_info_score, |x: &mut gtk::Label| {x.set_text(&format!("score : {}", pr.score))});
                update_elm!(ui.pr_info_elapsed_time, |x: &mut gtk::Label| {x.set_text(&format!("temps : {} s", pr.elapsed_time_s))});
                update_elm!(ui.pr_info_battery, |x: &mut gtk::Label| {x.set_text(&format!("batterie : {} mV", pr.battery_mv))});
            }
        });
        glib::Continue(false)
    };
    glib::idle_add(do_update);
}

pub fn init(rx_rdata: Receiver<RobotData>) {
    if gtk::init().is_err() {
        println!("Failed to initialize GTK.");
        return;
    }

    // First we get the file content.
    let glade_src = include_str!("glade/ui.glade");
    // Then we call the Builder call.
    let builder = gtk::Builder::new_from_string(glade_src);

    let window: gtk::Window = builder.get_object("mainWindow").unwrap();

    //init UI struct
    UII.with(|ui| {
        let mut ui = ui.borrow_mut();

        ui.rx_rdata = Some(rx_rdata);
        ui.pr_info_x = builder.get_object("pr_info_x");
        ui.pr_info_y = builder.get_object("pr_info_y");
        ui.pr_info_a = builder.get_object("pr_info_a");
        ui.pr_info_score = builder.get_object("pr_info_score");
        ui.pr_info_elapsed_time = builder.get_object("pr_info_elapsed_time");
        ui.pr_info_battery = builder.get_object("pr_info_battery");
    });

    window.connect_delete_event(|_, _| {
        // Stop the main loop.
        gtk::main_quit();
        // Let the default handler destroy the window.
        Inhibit(false)
    });

    window.show_all();
}

pub fn start() {
    gtk::main();
}
