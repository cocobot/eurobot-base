extern crate nphysics2d;

use std::{thread, time};
use std::sync::Arc;
use std::sync::Mutex;
use config_manager::config::ConfigManagerInstance;
use self::nphysics2d::world::World;
use super::brain::BrainInstance;
use com::ComInstance;

pub type PhysicsInstance = Arc<Mutex<Physics>>;

pub struct Physics {
    config: ConfigManagerInstance,
    com: ComInstance,
    brains: Vec<BrainInstance>,
}

impl Physics {
    pub fn new(config: ConfigManagerInstance, com: ComInstance) -> PhysicsInstance {
         let phys = Physics {
             config,
             com,
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
                let locked_instance = instance.lock().unwrap();
                for b in locked_instance.brains.iter() {
                    b.lock().unwrap().step(1000/60);
                }
                drop(locked_instance);

                world.step();

                let locked_instance = instance.lock().unwrap();
                let mut locked_com = locked_instance.com.lock().unwrap();
                let node = locked_com.get_node().unwrap();
                let mut locked_node = node.lock().unwrap();
                while let Some(frame) = locked_node.pop_tx_queue() {
                    debug!("TX {:?}", frame);
                    for b in locked_instance.brains.iter() {
                        b.lock().unwrap().can_tx(&frame);
                    }
                }
                drop(locked_node);
                drop(locked_com);
                drop(locked_instance);

                thread::sleep(delay);
            }
        });
    }

    pub fn add_brain(&mut self, brain: BrainInstance) {
        self.brains.push(brain);
    }
}
