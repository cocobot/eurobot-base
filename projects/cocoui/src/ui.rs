extern crate gtk;
extern crate glib;
extern crate cairo;
extern crate config;
extern crate time;
extern crate colored;

use std::cell::RefCell;
use std::sync::mpsc::Receiver;
use std::f64::consts::PI;
use std::collections::HashMap;
use ui::gtk::prelude::*;
use ui::config::{Config, Value};
use ui::time::PreciseTime;
use ui::colored::*;

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

struct UIBorder {
    pub color: [f64; 4],
    pub rectangle: [f64; 4],
}

impl UIBorder {
    pub fn draw(&self, ctx: &cairo::Context) {
        ctx.set_source_rgba(
            self.color[0],
            self.color[1],
            self.color[2],
            self.color[3],
            );
        ctx.rectangle(
            self.rectangle[0],
            self.rectangle[1],
            self.rectangle[2],
            self.rectangle[3],
            );
        ctx.fill();
        ctx.set_source_rgb(0.0, 0.0, 0.0);
        ctx.rectangle(
            self.rectangle[0],
            self.rectangle[1],
            self.rectangle[2],
            self.rectangle[3],
            );
        ctx.stroke();
    }
}

struct UICache {
    loaded: bool,

    //cached settings
    borders: Vec<UIBorder>,
    pathfinder_colors: HashMap<u32, [f64; 3]>,

    //display
    width: f64,
    height: f64,

    //surface
    borders_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_idx: usize,
    
}

impl UICache {
    pub fn new() -> UICache{
        UICache {
            loaded: false,
            borders: Vec::new(),
            pathfinder_colors: HashMap::new(),
            borders_surface: None,
            pathfinder_pr_surface: None,
            pathfinder_pr_idx: 0,
            width: -1.0,
            height: -1.0,
        }
    }

    pub fn invalidate_surface(&mut self, width: f64, height: f64) {
         if (width != self.width) || (height != self.height) {
             self.width = width;
             self.height = height;

             self.borders_surface = None;
             self.pathfinder_pr_surface = None;
         }
    }

    pub fn load(&mut self) {
        if !self.loaded {
            self.reload()
        }
    }
    
    pub fn reload(&mut self) {
        self.loaded = false;

        //load config file
        let mut config = Config::default();
        config.merge(config::File::with_name("config")).unwrap();

        //load borders
        self.borders = Vec::new();
        let borders = config.get("field.borders").and_then(Value::into_array);
        let borders = borders.as_ref().unwrap();
        for b in borders {
            let b = b.clone().into_table().unwrap();

            //parse color
            let mut parsed_color = [1.0; 4];
            let color = b.get("color").unwrap().clone().into_array().unwrap();
            for (i, value) in color.iter().enumerate() {
                parsed_color[i] = value.clone().into_float().unwrap();
            }

            //parse rectange
            let mut parsed_rectangle = [0.0; 4];
            let rect = b.get("rect").unwrap().clone().into_array().unwrap();
            for (i, value) in rect.iter().enumerate() {
                parsed_rectangle[i] = value.clone().into_float().unwrap();
            }

            self.borders.push(UIBorder {
                color: parsed_color,
                rectangle: parsed_rectangle,
            });
        }

        //load pathfinder colors
        self.pathfinder_colors = HashMap::new();
        let colors = config.get("pathfinder.grid_colors").and_then(Value::into_table);
        let colors = colors.as_ref().unwrap();
        for (key, val) in colors.iter() {
            let ikey = u32::from_str_radix(&key[2..], 16).unwrap();

            let mut parsed_color = [1.0; 3];
            let color = val.clone().into_array().unwrap();
            for (i, value) in color.iter().enumerate() {
                parsed_color[i] = value.clone().into_float().unwrap();
            }

            self.pathfinder_colors.insert(ikey, parsed_color);
        }

        self.loaded = true;
    }
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

    //cache
    cache: UICache,
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
              cache: UICache::new(),
          }
     }
}

pub fn draw_field(field: &gtk::DrawingArea, ctx: &cairo::Context) -> gtk::Inhibit {
    let start = PreciseTime::now();
    let mut inter1 = PreciseTime::now();
    let mut inter2 = PreciseTime::now();
    let mut inter3 = PreciseTime::now();
    let mut inter4 = PreciseTime::now();
    UII.with(|ui| {
        let mut ui = ui.borrow_mut();
        inter1 = PreciseTime::now();

        ui.cache.load();

        let width = field.get_allocated_width() as f64;
        let height = field.get_allocated_height() as f64;
        ui.cache.invalidate_surface(width, height);

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


        //draw borders
        if ui.cache.borders_surface.is_none() {
            let borders = cairo::ImageSurface::create(cairo::Format::ARgb32, width as i32, height as i32).unwrap();
            let ctx = cairo::Context::new(&borders);

            //set scale
            ctx.scale(coef * coef_x.signum(),coef * coef_y.signum());
            ctx.translate(-min_x, -min_y);

            ctx.set_line_width(1.0 / coef);
            ui.cache.borders.iter().for_each(|b| {
                b.draw(&ctx);
            });

            ui.cache.borders_surface = Some(borders);
        }
        ctx.set_operator(cairo::Operator::Over);
        ctx.set_source_surface(ui.cache.borders_surface.as_ref().unwrap(), 0.0, 0.0);
        ctx.paint();

        

        //draw robots
        if ui.pr.is_some() {
            ctx.save();
            //set scale
            ctx.scale(coef * coef_x.signum(),coef * coef_y.signum());
            ctx.translate(-min_x, -min_y);

            let mut pathfinder_pr_surface = None;
            let pathfinder_idx;

            {
                let pr = ui.pr.as_ref().unwrap();
                pathfinder_idx = pr.pathfinder_idx;

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

                //draw pathfinder
                inter2 = PreciseTime::now();
                inter3 = PreciseTime::now();
                if ui.cache.pathfinder_pr_surface.is_none() || ui.cache.pathfinder_pr_idx != pr.pathfinder_idx {
                    let cpathfinder_pr_surface = cairo::ImageSurface::create(cairo::Format::ARgb32, width as i32, height as i32).unwrap();
                    let ctx = cairo::Context::new(&cpathfinder_pr_surface);

                    //set scale
                    ctx.scale(coef * coef_x.signum(),coef * coef_y.signum());
                    ctx.translate(-min_x, -min_y);

                    let length = pr.pathfinder.len();
                    if length > 0 {
                        let width = pr.pathfinder[0].len();

                        let step_x = 3000.0 / (length as f64);
                        let step_y = 2000.0 / (width as f64);

                        for (i, line) in pr.pathfinder.iter().enumerate() {
                            for (j, value) in line.iter().enumerate() {
                                let sx = -1500.0 + (i as f64) * step_x;
                                let sy = 1000.0 - ((j + 1) as f64) * step_y;
                                ctx.rectangle(sx, sy, step_x, step_y);

                                match ui.cache.pathfinder_colors.get(&(*value as u32)) {
                                    Some(&color) => {
                                        ctx.set_source_rgba(color[0], color[1], color[2], 0.3);
                                    }
                                    _ => {
                                        ctx.set_source_rgba(0.0, 0.0, 0.0, 1.0);
                                        eprintln!("{} no color defined for 0x{:X}", "Pathfinder:".yellow(), value);
                                    }
                                }
                                ctx.fill();

                                ctx.rectangle(sx, sy, step_x, step_y);
                                ctx.set_line_width(2.0);
                                ctx.set_source_rgba(0.0, 0.0, 0.0, 1.0);
                                ctx.stroke();
                            }
                        }
                    }
                    pathfinder_pr_surface = Some(cpathfinder_pr_surface);
                }
                if pathfinder_pr_surface.is_some() {
                    ctx.set_operator(cairo::Operator::Over);
                    ctx.set_source_surface(pathfinder_pr_surface.as_ref().unwrap(), 0.0, 0.0);
                    ctx.paint();
                }
                else if ui.cache.pathfinder_pr_surface.is_some() {
                    ctx.set_operator(cairo::Operator::Over);
                    ctx.set_source_surface(ui.cache.pathfinder_pr_surface.as_ref().unwrap(), 0.0, 0.0);
                    ctx.paint();
                }
            }
            if pathfinder_pr_surface.is_some() {
                ui.cache.pathfinder_pr_idx = pathfinder_idx;
                ui.cache.pathfinder_pr_surface = pathfinder_pr_surface;
            }
        }

        inter4 = PreciseTime::now();
    });

    let end = PreciseTime::now();
    //println!("draw_field exec time:   {} ms", start.to(end) * 1000);
    //println!("draw_field exec inter1: {} ms", start.to(inter1) * 1000);
    //println!("draw_field exec inter2: {} ms", inter1.to(inter2) * 1000);
    //println!("draw_field exec inter3: {} ms", inter2.to(inter3) * 1000);
    //println!("draw_field exec inter4: {} ms", inter3.to(inter4) * 1000);
    //println!("draw_field exec inter3: {} ms", inter3.to(end) * 1000);
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
