extern crate gtk;
extern crate glib;
extern crate cairo;
extern crate config;

use std::cell::RefCell;
use std::sync::mpsc::Receiver;
use std::f64::consts::PI;
use ui::gtk::prelude::*;
use ui::config::{Config, Value};

use robot::RobotData;

macro_rules! update_elm {
    ($a: expr, $b: expr) => {
        if $a.is_some() {
            $b($a.as_mut().unwrap());
        }
    };
}

thread_local!(
    static UII: RefCell<UI> = RefCell::new(UI::new());
);

lazy_static! {
    static ref SETTINGS: Config = {
        let mut config = Config::default();
        config.merge(config::File::with_name("config")).unwrap();

        config
    };
}

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
    field: Option<gtk::DrawingArea>,

    //data
    pr: Option<RobotData>,
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
              field: None,
              pr: None,
          }
     }
}

pub fn draw_field(field: &gtk::DrawingArea, ctx: &cairo::Context) -> gtk::Inhibit {
    UII.with(|ui| {
        let ui = ui.borrow();

        let width = field.get_allocated_width() as f64;
        let height = field.get_allocated_height() as f64;
        let field = SETTINGS.get("field").and_then(Value::into_table).unwrap();

        let min_x = field.get("min_x").unwrap().clone().into_float().unwrap();
        let max_x = field.get("max_x").unwrap().clone().into_float().unwrap();
        let min_y = field.get("min_y").unwrap().clone().into_float().unwrap();
        let max_y = field.get("max_y").unwrap().clone().into_float().unwrap();

        let coef_x = width / (max_x - min_x);
        let coef_y = height / (max_y - min_y);
        let mut coef = coef_x.abs();

        if coef_x.abs() > coef_y.abs() {
            coef = coef_y.abs();
        }

        //set scale
        ctx.scale(coef * coef_x.signum(),coef * coef_y.signum());
        ctx.translate(-min_x, -min_y);


        //draw borders
        let borders = SETTINGS.get("field.borders").and_then(Value::into_array);
        let borders = borders.as_ref().unwrap();
        for b in borders {
            let b = b.clone().into_table().unwrap();

            //set color
            let color = b.get("color").unwrap().clone().into_array().unwrap();
            let color: Vec<f64> = color.iter().map(|x|-> f64 {
                x.clone().into_float().unwrap()
            }).collect();
            ctx.set_source_rgb(*color.get(0).unwrap(), *color.get(1).unwrap(), *color.get(2).unwrap());

            //draw borders
            let rect = b.get("rect").unwrap().clone().into_array().unwrap();
            let rect: Vec<f64> = rect.iter().map(|x|-> f64 {
                x.clone().into_float().unwrap()
            }).collect();
            ctx.set_source_rgb(*color.get(0).unwrap(), *color.get(1).unwrap(), *color.get(2).unwrap());
            ctx.rectangle(
                *rect.get(0).unwrap(),
                *rect.get(1).unwrap(),
                *rect.get(2).unwrap(),
                *rect.get(3).unwrap()
                );
            ctx.fill();

            ctx.set_line_width(1.0 / coef);
            ctx.set_source_rgb(0.0, 0.0, 0.0);
            ctx.rectangle(
                *rect.get(0).unwrap(),
                *rect.get(1).unwrap(),
                *rect.get(2).unwrap(),
                *rect.get(3).unwrap()
                );
            ctx.stroke();
        }


        //draw robots
        if ui.pr.is_some() {
            ctx.save();

            let pr = ui.pr.as_ref().unwrap();
            ctx.translate(pr.x_mm, pr.y_mm);
            ctx.rotate(pr.a_deg * PI / 180.0);

            let prshape = SETTINGS.get("pr.shape").and_then(Value::into_array).unwrap();
            let prshape: Vec<[f64; 2]> = prshape.iter().map(|x|-> [f64; 2] {
                let pt = x.clone().into_array().unwrap();
                [
                    pt.get(0).unwrap().clone().into_float().unwrap(),
                    pt.get(1).unwrap().clone().into_float().unwrap()
                ]
            }).collect();

            let mut iter = prshape.iter();
            let first = iter.next().unwrap();
            ctx.new_path();
            ctx.move_to(first[0], first[1]);
            iter.for_each(|x| {
                ctx.line_to(x[0], x[1]);
            });
            ctx.close_path();

            let fill = SETTINGS.get("pr.fill").and_then(Value::into_array).unwrap();
            let fill: Vec<f64> = fill.iter().map(|x|-> f64 {
                x.clone().into_float().unwrap()
            }).collect();
            ctx.set_source_rgb(*fill.get(0).unwrap(), *fill.get(1).unwrap(), *fill.get(2).unwrap());
            ctx.fill();

            let mut iter = prshape.iter();
            let first = iter.next().unwrap();
            ctx.new_path();
            ctx.move_to(first[0], first[1]);
            iter.for_each(|x| {
                ctx.line_to(x[0], x[1]);
            });
            ctx.close_path();

            let stroke = SETTINGS.get("pr.stroke").and_then(Value::into_array).unwrap();
            let stroke: Vec<f64> = stroke.iter().map(|x|-> f64 {
                x.clone().into_float().unwrap()
            }).collect();
            ctx.set_line_width(1.0 / coef);
            ctx.set_source_rgb(*stroke.get(0).unwrap(), *stroke.get(1).unwrap(), *stroke.get(2).unwrap());
            ctx.stroke();

            ctx.restore();

            /*
            //draw pathfinder
            let length = pr.pathfinder.len();
            if length > 0 {
                let width = pr.pathfinder[0].len();
                for (i, line) in pr.pathfinder.iter().enumerate() {
                    for (j, value) in line.iter().enumerate() {
                        ctx.rectangle((j * 5) as f64, (i * 5) as f64, 5.0, 5.0);
                        ctx.set_source_rgba(1.0, 0.0, 0.0, 0.5);
                        ctx.fill();
                    }
                }
            }
            */
        }

    });

    gtk::Inhibit(false)
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
                ui.pr = pr.clone();
                let pr = pr.unwrap();
                update_elm!(ui.pr_info_x, |x: &mut gtk::Label| {x.set_text(&format!("x : {:.0} mm", pr.x_mm))});
                update_elm!(ui.pr_info_y, |x: &mut gtk::Label| {x.set_text(&format!("y : {:.0} mm", pr.y_mm))});
                update_elm!(ui.pr_info_a, |x: &mut gtk::Label| {x.set_text(&format!("a : {:.0} °", pr.a_deg))});
                update_elm!(ui.pr_info_score, |x: &mut gtk::Label| {x.set_text(&format!("score : {}", pr.score))});
                update_elm!(ui.pr_info_elapsed_time, |x: &mut gtk::Label| {x.set_text(&format!("temps : {} s", pr.elapsed_time_s))});
                update_elm!(ui.pr_info_battery, |x: &mut gtk::Label| {x.set_text(&format!("batterie : {} mV", pr.battery_mv))});
            }

            update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.queue_draw()});
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
        ui.field = builder.get_object("field");

        update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.connect_draw(draw_field)});
    });

    window.connect_delete_event(|_, _| {
        // Stop the main loop.
        gtk::main_quit();
        // Let the default handler destroy the window.
        Inhibit(false)
    });

    window.set_default_size(1200, 600);

    window.show_all();
}

pub fn start() {
    gtk::main();
}
