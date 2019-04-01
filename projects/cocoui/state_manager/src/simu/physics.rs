extern crate canars;
extern crate nphysics2d;

use self::canars::CANFrame;
use self::nphysics2d::world::World;
use config_manager::config::ConfigManagerInstance;
use simu::brain::BrainInstance;
use std::sync::mpsc::Receiver;
use std::sync::Arc;
use std::sync::Mutex;
use std::time::Duration;
use std::{thread, time};

pub type PhysicsInstance = Arc<Mutex<Physics>>;

pub struct Physics {
    _config: ConfigManagerInstance,
    tx_can: Receiver<CANFrame>,
    brains: Vec<BrainInstance>,
}

impl Physics {
    pub fn new(config: ConfigManagerInstance, tx_can: Receiver<CANFrame>) -> PhysicsInstance {
        let phys = Physics {
            _config: config,
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

            loop {
                let locked_instance = instance.lock().unwrap();
                for b in locked_instance.brains.iter() {
                    b.lock().unwrap().step(1000 / 60);
                }
                drop(locked_instance);

                world.step();

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
