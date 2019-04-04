extern crate canars;
extern crate nalgebra;
extern crate ncollide2d;
extern crate nphysics2d;

use super::brain::BrainInstance;
use crate::simu::physics::canars::CANFrame;
use crate::simu::physics::nalgebra::geometry::Point2;
use crate::simu::physics::nalgebra::Vector2;
use crate::simu::physics::ncollide2d::shape::{ConvexPolygon, ShapeHandle};
use crate::simu::physics::nphysics2d::object::ColliderDesc;
use crate::simu::physics::nphysics2d::object::RigidBodyDesc;
use crate::simu::physics::nphysics2d::world::World;
use crate::simu::physics::nphysics2d::math::{Force, ForceType};
use crate::simu::physics::nphysics2d::object::Body;
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
                for (i, b) in locked_instance.brains.iter().enumerate() {
                    let mut b = b.lock().unwrap();

                    b.step(1000 / 60);

                    if let Some(x) = b.force_x {
                        let body = world.rigid_body_mut(*robots.get(i).unwrap()).unwrap();
                        let mut pos = body.position().clone();
                        pos.translation.vector.x = x as f32;
                        body.set_position(pos);
                        b.force_x = None;
                    }

                    if let Some(y) = b.force_y {
                        let body = world.rigid_body_mut(*robots.get(i).unwrap()).unwrap();
                        let mut pos = body.position().clone();
                        pos.translation.vector.y = y as f32;
                        body.set_position(pos);
                        b.force_y = None;
                    }

                    if let Some(a) = b.force_a {
                        let body = world.rigid_body_mut(*robots.get(i).unwrap()).unwrap();
                        let mut pos = body.position().clone();
                        //pos.translation.vector.x = x as f32;
                        //body.set_position(pos);
                        b.force_a = None;
                    }

                }
                drop(locked_instance);

                //update velocity changes
                for (i, robot) in robots.iter().enumerate() {
                    let body = world.rigid_body_mut(*robot).unwrap();
                    body.set_linear_velocity(body.position().rotation.transform_vector(&Vector2::x()) * 200.0);
                    body.set_angular_velocity(-0.1);
                }

                world.step();

                //update robots position
                let mut locked_state = state_instance.lock().unwrap();
                let mut state = locked_state.get_state_mut();
                for (i, robot) in robots.iter().enumerate() {
                    let pos = world.rigid_body(*robot).unwrap().position();
                    let position = pos.translation.vector;

                    state.robots[i].simu = true;
                    state.robots[i].simu_x = position[0] as f64;
                    state.robots[i].simu_y = position[1] as f64;
                    state.robots[i].simu_a = (pos.rotation.scaled_axis()[0] * 180.0 / std::f32::consts::PI) as f64;
                }
                drop(state);
                drop(locked_state);

                let mut waiting_tx = Vec::new();
                let locked_instance = instance.lock().unwrap();
                while let Ok(frame) = locked_instance
                    .tx_can
                    .recv_timeout(Duration::from_millis(0))
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
