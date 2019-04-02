extern crate gtk;
extern crate gdk;

use std::cell::RefCell;
use state_manager::state::StateManagerInstance;
use ui::gtk::prelude::*;
use std::collections::HashMap;
use config_manager::config::ConfigManagerInstance;

mod boards;

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

#[derive(Debug)]
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
    config: Option<ConfigManagerInstance>,

    //cached settings
    min_x: f64,
    min_y: f64,
    max_x: f64,
    max_y: f64,
    borders: Vec<UIBorder>,
    pathfinder_colors: HashMap<u32, [f64; 3]>,
    pathfinder_color_opacity: f64,
    pathfinder_grid_color: [f64; 4],

    //display
    width: f64,
    height: f64,

    //surface
    borders_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_idx: usize,

}

impl UICache {
    pub fn new() -> UICache {
        UICache {
            loaded: false,
            config: None,
            min_x: 0.0,
            min_y: 0.0,
            max_x: 1.0,
            max_y: 1.0,
            borders: Vec::new(),
            pathfinder_colors: HashMap::new(),
            pathfinder_color_opacity: 1.0,
            pathfinder_grid_color: [1.0; 4],
            borders_surface: None,
            pathfinder_pr_surface: None,
            pathfinder_pr_idx: 0,
            width: -1.0,
            height: -1.0,
        }
    }

    pub fn set_config(&mut self, config: ConfigManagerInstance) {
        self.config = Some(config);
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
        if let Some(config) = self.config.as_ref() {
            let config = config.lock().unwrap();

            //load field maxima
            self.min_x = config.field.min_x;
            self.max_x = config.field.max_x;
            self.min_y = config.field.min_y;
            self.max_y = config.field.max_y;

            //load borders
            self.borders = Vec::new();
            for b in &config.field.borders {
                //parse color
                let mut parsed_color = [1.0; 4];
                for (i, value) in b.color.iter().enumerate() {
                    parsed_color[i] = *value;
                }

                //parse rectange
                let mut parsed_rectangle = [0.0; 4];
                for (i, value) in b.rect.iter().enumerate() {
                    parsed_rectangle[i] = *value
                }

                self.borders.push(UIBorder {
                    color: parsed_color,
                    rectangle: parsed_rectangle,
                });
            }


            /*
            //load pathfinder settings
            let pathfinder = config.get("pathfinder").and_then(Value::into_table);
            let pathfinder = pathfinder.as_ref().unwrap();

            let pathfinder_color_opacity = pathfinder.get("color_opacity").clone().unwrap();
            let pathfinder_color_opacity = pathfinder_color_opacity.clone().into_float().unwrap();
            self.pathfinder_color_opacity = pathfinder_color_opacity;

            let pathfinder_grid_color = pathfinder.get("grid_color").clone().unwrap();
            let pathfinder_grid_color = pathfinder_grid_color.clone().into_array().unwrap();
            for (i, value) in pathfinder_grid_color.iter().enumerate() {
            self.pathfinder_grid_color[i] = value.clone().into_float().unwrap();
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

            */
            self.loaded = true;
        }
    }
}

struct UI {
    state: Option<StateManagerInstance>,
    field: Option<gtk::DrawingArea>,

    //cache
    cache: UICache,
}

impl UI {
    fn new() -> UI {
        UI {
            state: None,
            field: None,
            cache: UICache::new(),
        }
    }
}

pub fn draw_field(field: &gtk::DrawingArea, ctx: &cairo::Context) -> gtk::Inhibit {
    UII.with(|ui| {
        let mut ui = ui.borrow_mut();

        ui.cache.load();

        let width = field.get_allocated_width() as f64;
        let height = field.get_allocated_height() as f64;
        ui.cache.invalidate_surface(width, height);

        let min_x = ui.cache.min_x;
        let max_x = ui.cache.max_x;
        let min_y = ui.cache.min_y;
        let max_y = ui.cache.max_y;

        let coef_x = width / (max_x - min_x);
        let coef_y = height / (max_y - min_y);
        let mut coef = coef_x.abs();

        if coef_x.abs() > coef_y.abs() {
            coef = coef_y.abs();
        }

        //draw borders
        if ui.cache.borders_surface.is_none() {
            info!("T2");
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

        /*


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
                                        ctx.set_source_rgba(color[0], color[1], color[2], ui.cache.pathfinder_color_opacity);
                                    }
                                    _ => {
                                        ctx.set_source_rgba(0.0, 0.0, 0.0, 1.0);
                                        eprintln!("{} no color defined for 0x{:X}", "Pathfinder:".yellow(), value);
                                    }
                                }
                                ctx.fill();

                                ctx.rectangle(sx, sy, step_x, step_y);
                                ctx.set_line_width(2.0);
                                ctx.set_source_rgba(
                                    ui.cache.pathfinder_grid_color[0],
                                    ui.cache.pathfinder_grid_color[1],
                                    ui.cache.pathfinder_grid_color[2],
                                    ui.cache.pathfinder_grid_color[3]
                                    );
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
    */
    });

    gtk::Inhibit(false)
}

fn update() {
    gtk::timeout_add(100, || {
        UII.with(|ui| {
            let mut ui = ui.borrow_mut();

            update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.queue_draw()});
        });
        gtk::Continue(true)
    });
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
            gdk::enums::key::D => {
                debug!("KEY D");
                UII.with(|ui| {
                    let ui = ui.borrow();

                    let state = ui.state.as_ref().unwrap().lock().unwrap();
                    state.command("restart 127");
                });
            }

            _ => {},
        };

        Inhibit(false)
    });
}

pub fn init(config: ConfigManagerInstance, state: StateManagerInstance) {
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
        
        ui.cache.set_config(config);

        ui.field = builder.get_object("field");
        ui.state = Some(state.clone());

        update_elm!(ui.field, |x: &mut gtk::DrawingArea| {x.connect_draw(draw_field)});
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
