extern crate gdk;
extern crate gdk_pixbuf;
extern crate gtk;

use crate::config_manager::config::ConfigManagerInstance;
use crate::state_manager::state::State;
use crate::state_manager::state::StateManagerInstance;
use crate::ui::gdk::ContextExt;
use crate::ui::gdk_pixbuf::Pixbuf;
use crate::ui::gtk::prelude::*;
use std::cell::RefCell;
use std::collections::HashMap;
use std::f64::consts::PI;

mod boards;
mod motors;
mod meca;

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
        ctx.set_source_rgba(self.color[0], self.color[1], self.color[2], self.color[3]);
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

struct UIRobot {
    pub shape: Vec<[f64; 2]>,
    pub fill: [f64; 4],
    pub stroke: [f64; 4],
}

impl UIRobot {
    pub fn load(robot: &config_manager::defs::Robot) -> UIRobot {
        let mut shape = Vec::new();
        for (_, value) in robot.shape.iter().enumerate() {
            shape.push(*value);
        }

        let mut fill = [1.0; 4];
        for (i, value) in robot.fill.iter().enumerate() {
            fill[i] = *value;
        }

        let mut stroke = [1.0; 4];
        for (i, value) in robot.stroke.iter().enumerate() {
            stroke[i] = *value;
        }

        UIRobot {
            shape,
            fill,
            stroke,
        }
    }

    pub fn draw(&self, ctx: &cairo::Context, simulation: bool) {

        let opacity_factor = if simulation { 0.3 } else { 1.0 };

        //create path for fill
        let mut iter = self.shape.iter();
        let first = iter.next().unwrap();
        ctx.new_path();
        ctx.move_to(first[0], first[1]);
        iter.for_each(|x| {
            ctx.line_to(x[0], x[1]);
        });
        ctx.close_path();

        //fill
        ctx.set_source_rgba(self.fill[0], self.fill[1], self.fill[2], self.fill[3] * opacity_factor);
        ctx.fill();

        //create path for stroke
        let mut iter = self.shape.iter();
        let first = iter.next().unwrap();
        ctx.new_path();
        ctx.move_to(first[0], first[1]);
        iter.for_each(|x| {
            ctx.line_to(x[0], x[1]);
        });
        ctx.close_path();

        //stroke
        ctx.set_source_rgba(
            self.stroke[0],
            self.stroke[1],
            self.stroke[2],
            self.stroke[3] * opacity_factor,
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
    background_path: String,

    //display
    width: f64,
    height: f64,

    //surface
    borders_surface: Option<cairo::ImageSurface>,
    background_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_surface: Option<cairo::ImageSurface>,
    pathfinder_pr_idx: usize,

    //robots
    robots: Vec<UIRobot>,
    //state
    state: Option<State>,
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
            background_path: String::new(),
            borders_surface: None,
            background_surface: None,
            pathfinder_pr_surface: None,
            pathfinder_pr_idx: 0,
            width: -1.0,
            height: -1.0,
            state: None,
            robots: Vec::new(),
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
            self.background_surface = None;
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

            //load background
            self.background_path = config.field.background.clone();

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

            //load robots
            self.robots.push(UIRobot::load(&config.robots.main));
            self.robots.push(UIRobot::load(&config.robots.pmi));

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


struct RobotPanelUI {
  info_x: Option<gtk::Label>,  
  info_y: Option<gtk::Label>,  
  info_a: Option<gtk::Label>,  
}

impl RobotPanelUI {
    pub fn new() -> RobotPanelUI {
        RobotPanelUI {
          info_x: None,
          info_y: None,
          info_a: None,
        }
    }
}

struct UI {
    state: Option<StateManagerInstance>,
    field: Option<gtk::DrawingArea>,

    robots: [RobotPanelUI; 2],

    //cache
    cache: UICache,
}

impl UI {
    fn new() -> UI {
        UI {
            state: None,
            field: None,
            cache: UICache::new(),
            robots: [RobotPanelUI::new(), RobotPanelUI::new()],
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

        //draw background

        if let Some(background_surface) = ui.cache.background_surface.as_ref() {
            ctx.set_operator(cairo::Operator::Over);
            ctx.set_source_surface(background_surface, 0.0, 0.0);
            ctx.paint();
        } else {
            match Pixbuf::new_from_file(ui.cache.background_path.to_owned()) {
                Ok(image) => {
                    let background = cairo::ImageSurface::create(
                        cairo::Format::ARgb32,
                        width as i32,
                        height as i32,
                    )
                    .unwrap();
                    let ctx = cairo::Context::new(&background);
                    ctx.scale(coef, coef);
                    ctx.translate(22.0, 22.0);
                    ctx.set_source_pixbuf(&image, 0.0, 0.0);
                    ctx.rectangle(0.0, 0.0, 3000.0, 2000.0);
                    ctx.fill();

                    ui.cache.background_surface = Some(background);
                }
                Err(r) => {
                    error!("background: {:?}", r);
                }
            };
        }

        //draw borders
        if ui.cache.borders_surface.is_none() {
            let borders =
                cairo::ImageSurface::create(cairo::Format::ARgb32, width as i32, height as i32)
                    .unwrap();
            let ctx = cairo::Context::new(&borders);

            //set scale
            ctx.scale(coef * coef_x.signum(), coef * coef_y.signum());
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

        if let Some(state) = &ui.cache.state {
            ctx.save();
            //set scale
            ctx.scale(coef * coef_x.signum(), coef * coef_y.signum());
            ctx.translate(-min_x, -min_y);

            //draw robots
            for i in 0..2 {
                let robot = state.robots[i];
                ctx.save();
                ctx.translate(robot.x, robot.y);
                ctx.rotate(robot.a * PI / 180.0);
                ui.cache.robots[i].draw(ctx, false);
                ctx.restore();
            }
            
            //draw robots
            for i in 0..2 {
                let robot = state.robots[i];
                ctx.save();
                ctx.translate(robot.simu_x, robot.simu_y);
                ctx.rotate(robot.simu_a * PI / 180.0);
                ui.cache.robots[i].draw(ctx, true);
                ctx.restore();
            }

            /*
            let mut pathfinder_pr_surface = None;
            let pathfinder_idx;

            {
                let pr = ui.pr.as_ref().unwrap();
                pathfinder_idx = pr.pathfinder_idx;



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
            */
            ctx.restore();
        }
    });

    gtk::Inhibit(false)
}

fn update() {
    gtk::timeout_add(100, || {
        UII.with(|ui| {
            let mut ui = ui.borrow_mut();

            //update state
            let mut state_cpy = None;
            let mut state_cpy2 = None;
            if let Some(state) = ui.state.as_ref() {
                let state = state.lock().unwrap();
                state_cpy = Some(state.get_state());
                state_cpy2 = Some(state.get_state());
            }
            ui.cache.state = state_cpy;

            //update field canvas
            update_elm!(ui.field, |x: &mut gtk::DrawingArea| x.queue_draw());

            let state_cpy2 = state_cpy2.unwrap();
            //update robot panels
            ui.robots[0].info_x.as_mut().unwrap().set_text(&format!("x: {:.0}", state_cpy2.robots[0].x));
            ui.robots[0].info_y.as_mut().unwrap().set_text(&format!("y: {:.0}", state_cpy2.robots[0].y));
            ui.robots[0].info_a.as_mut().unwrap().set_text(&format!("a: {:.0}", state_cpy2.robots[0].a));
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
            }
            gdk::enums::key::O => {
                debug!("KEY O");
                UII.with(|ui| {
                    let ui = ui.borrow();

                    let state = ui.state.as_ref().unwrap().lock().unwrap();
                    state.command("pgm 12");
                });
            }
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

            _ => {}
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
    motors::init(state.clone());
    meca::init(state.clone());

    let glade_src = include_str!("glade/ui.glade");
    let builder = gtk::Builder::new_from_string(glade_src);
    let window: gtk::Window = builder.get_object("mainWindow").unwrap();

    //init UI struct
    UII.with(|ui| {
        let mut ui = ui.borrow_mut();

        ui.cache.set_config(config);

        ui.field = builder.get_object("field");
        ui.state = Some(state.clone());

        //robots panels
        let lbl : gtk::Label = builder.get_object("pr_info_x").unwrap();
        ui.robots[0].info_x = Some(lbl);
        let lbl : gtk::Label = builder.get_object("pr_info_y").unwrap();
        ui.robots[0].info_y = Some(lbl);
        let lbl : gtk::Label = builder.get_object("pr_info_a").unwrap();
        ui.robots[0].info_a = Some(lbl);

        let btn : gtk::Button = builder.get_object("pmotor").unwrap();
        btn.connect_clicked( |_| {
            motors::show(true);
        });

        let btn : gtk::Button = builder.get_object("pmeca").unwrap();
        btn.connect_clicked( |_| {
            meca::show(true);
        });

        update_elm!(ui.field, |x: &mut gtk::DrawingArea| x
            .connect_draw(draw_field));
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
