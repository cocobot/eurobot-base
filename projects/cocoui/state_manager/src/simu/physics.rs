extern crate canars;
extern crate nphysics2d;
extern crate ncollide2d;
extern crate nalgebra;

use crate::simu::physics::canars::CANFrame;
use crate::simu::physics::nphysics2d::world::World;
use crate::simu::physics::ncollide2d::shape::{ShapeHandle, ConvexPolygon};
use crate::simu::physics::nphysics2d::object::ColliderDesc;
use crate::simu::physics::nphysics2d::object::RigidBodyDesc;
use crate::simu::physics::nalgebra::geometry::Point2;
use config_manager::config::ConfigManagerInstance;
use super::brain::BrainInstance;
use std::sync::mpsc::Receiver;
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
    pub fn new(config: ConfigManagerInstance, tx_can: Receiver<CANFrame>) -> PhysicsInstance {
        let phys = Physics {
            config: config,
            tx_can,
            brains: Vec::new(),
        };

        let instance = Arc::new(Mutex::new(phys));

        Physics::start_simulation(instance.clone());

        instance
    }

    fn start_simulation(instance: PhysicsInstance) {
        thread::spawn(move || {
            let mut world: World<f32> = World::new();
            let delay = time::Duration::from_millis(1000 / 60);

            println!("Z1");
            let locked_instance = instance.lock().unwrap();
            let config = locked_instance.config.clone();
            drop(locked_instance);
            println!("Z2");
            let config = config.lock().unwrap();
            println!("Z3");

            //create robots body
            let rdata  = [&config.robots.main, &config.robots.pmi];
            let mut robots = Vec::new();
            for (i, robot) in rdata.iter().enumerate() {
                let mut points = Vec::new();
                for pt in robot.shape.iter() {
                    points.push(Point2::new(pt[0] as f32, pt[1] as f32));
                }
                let shape = ConvexPolygon::try_from_points(points.iter().as_slice()).expect("Convex hull computation failed.");
                let collider = ColliderDesc::new(ShapeHandle::new(shape)).density(1.0);

                //create robot rigid body
                let body_center = RigidBodyDesc::new()
                    .gravity_enabled(false)
                    .collider(&collider)
                    .name(format!("r{}", i))
                    .build(&mut world);

                robots.push(body_center.handle());
            }
            println!("Z5");

            drop(config);
            println!("Z7");

            loop {
                let locked_instance = instance.lock().unwrap();
                for b in locked_instance.brains.iter() {
                    b.lock().unwrap().step(1000 / 60);
                }
                drop(locked_instance);

                world.step();

                //update robots position
                for r in robots.iter() {
                    debug!("Robot: {:?}", world.rigid_body(*r).unwrap().position());
                }

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
        self.brains.iter()
            .position(|b| Arc::ptr_eq(&b, &brain))
            .map(|e| self.brains.remove(e));
    }

}
