extern crate canars;
extern crate nalgebra;
extern crate ncollide2d;
extern crate nphysics2d;

use super::brain::BrainInstance;
use crate::simu::physics::canars::CANFrame;
use crate::simu::physics::nalgebra::geometry::Point2;
use crate::simu::physics::nalgebra::{Vector2, Isometry2};
use crate::simu::physics::ncollide2d::shape::{ConvexPolygon, ShapeHandle};
use crate::simu::physics::nphysics2d::object::ColliderDesc;
use crate::simu::physics::nphysics2d::object::RigidBodyDesc;
use crate::simu::physics::nphysics2d::world::World;
//use crate::simu::physics::nphysics2d::object::Body;
use crate::state::StateManagerInstance;
use config_manager::config::ConfigManagerInstance;
use crossbeam_channel::Receiver;
use std::sync::Arc;
use std::sync::Mutex;
use std::time::Duration;
use std::{thread, time};

pub type PhysicsInstance = Arc<Mutex<Physics>>;

pub struct Physics {
    config: ConfigManagerInstance,
    tx_can: Receiver<CANFrame>,
    brains: Vec<BrainInstance>,
}

impl Physics {
    pub fn new(
        config: ConfigManagerInstance,
        tx_can: Receiver<CANFrame>,
        state: StateManagerInstance,
        ) -> PhysicsInstance {
        let phys = Physics {
            config: config,
            tx_can,
            brains: Vec::new(),
        };

        let instance = Arc::new(Mutex::new(phys));

        Physics::start_simulation(instance.clone(), state);

        instance
    }

    fn start_simulation(instance: PhysicsInstance, state_instance: StateManagerInstance) {
        thread::spawn(move || {
            let mut world: World<f32> = World::new();
            let delay = time::Duration::from_millis(1000 / 60);

            let locked_instance = instance.lock().unwrap();
            let config = locked_instance.config.clone();
            drop(locked_instance);
            let config = config.lock().unwrap();

            //create robots body
            let rdata = [&config.robots.main, &config.robots.pmi];
            //let rdata  = [&config.robots.main];
            let mut robots = Vec::new();
            for (i, robot) in rdata.iter().enumerate() {
                let mut points = Vec::new();
                for pt in robot.shape.iter() {
                    points.push(Point2::new(pt[0] as f32, pt[1] as f32));
                }
                let shape = ConvexPolygon::try_from_points(points.iter().as_slice())
                    .expect("Convex hull computation failed.");
                let collider = ColliderDesc::new(ShapeHandle::new(shape)).density(1.0);

                //create robot rigid body
                let body_center = RigidBodyDesc::new()
                    .gravity_enabled(false)
                    .collider(&collider)
                    .name(format!("r{}", i))
                    .build(&mut world);

                robots.push(body_center.handle());
            }

            //create borders
            for border in config.field.borders.iter() {
                let mut points = Vec::new();
                points.push(Point2::new(
                        border.rect[0] as f32, 
                        border.rect[1] as f32));
                points.push(Point2::new(
                        border.rect[0] as f32, 
                        (border.rect[1] + border.rect[3]) as f32));
                points.push(Point2::new(
                        (border.rect[0] + border.rect[2]) as f32, 
                        (border.rect[1] + border.rect[3]) as f32));
                points.push(Point2::new(
                        (border.rect[0] + border.rect[2]) as f32, 
                        border.rect[1] as f32));

                let shape = ConvexPolygon::try_from_points(points.iter().as_slice())
                    .expect("Convex hull computation failed.");
                let collider = ColliderDesc::new(ShapeHandle::new(shape));
                collider.build(&mut world);
            }

            drop(config);

            loop {
                let locked_instance = instance.lock().unwrap();
                for (_i, b) in locked_instance.brains.iter().enumerate() {
                    let mut b = b.lock().unwrap();
                    let distance = (b.speed_d) * b.tick_per_meter / 1000.0;
                    let angle = (b.speed_a) * b.tick_per_180deg / std::f32::consts::PI;

                    b.timers[5].adder += (distance + angle) / 2.0;
                    b.timers[2].adder += (distance - angle) / 2.0;

                    b.step(1000 / 60);
                }
                drop(locked_instance);


                let mut waiting_tx = Vec::new();
                let locked_instance = instance.lock().unwrap();
                while let Ok(frame) = locked_instance
                    .tx_can
                        .recv_timeout(Duration::from_millis(0))
                        {
                            waiting_tx.push(frame);
                        }
                drop(locked_instance);

                let mut waiting_tx = Vec::new();
                let locked_instance = instance.lock().unwrap();
                while let Ok(frame) = locked_instance
                    .tx_can
                        .recv_timeout(Duration::from_millis(1))
                        {
                            waiting_tx.push(frame);
                        }
                drop(locked_instance);

                let locked_instance = instance.lock().unwrap();
                for b in locked_instance.brains.iter() {
                    let mut b = b.lock().unwrap();
                    for tx in waiting_tx.iter() {
                        b.can_tx(&tx);
                    }
                }
                drop(locked_instance);


                thread::sleep(delay);

            }
        });
    }

    pub fn add_brain(&mut self, brain: BrainInstance) {
        self.brains.push(brain);
    }

    pub fn remove_brain(&mut self, brain: BrainInstance) {
        self.brains
            .iter()
            .position(|b| Arc::ptr_eq(&b, &brain))
            .map(|e| self.brains.remove(e));
    }
}
