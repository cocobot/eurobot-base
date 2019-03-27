extern crate nphysics2d;

use std::{thread, time};
use std::sync::Arc;
use std::sync::Mutex;
use config_manager::config::ConfigManagerInstance;
use self::nphysics2d::world::World;
use super::brain::BrainInstance;

pub type PhysicsInstance = Arc<Mutex<Physics>>;

pub struct Physics {
    config: ConfigManagerInstance,
    brains: Vec<BrainInstance>,
}

impl Physics {
    pub fn new(config: ConfigManagerInstance) -> PhysicsInstance {
         let phys = Physics {
             config,
             brains: Vec::new(),
         };

         let instance = Arc::new(Mutex::new(phys));

         Physics::start_simulation(instance.clone());

         instance
    }

    fn start_simulation(instance: PhysicsInstance) {
        thread::spawn(move || {
            let mut world : World<f32> = World::new();
            let delay = time::Duration::from_millis(1000/60);
            loop {
                world.step();

                let locked_instance = instance.lock().unwrap();
                for b in locked_instance.brains.iter() {
                    b.lock().unwrap().step(1000/60);
                }

                thread::sleep(delay);
            }
        });
    }

    pub fn add_brain(&mut self, brain: BrainInstance) {
        self.brains.push(brain);
    }
}
