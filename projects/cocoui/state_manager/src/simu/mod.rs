mod physics;
mod brain;

use std::sync::Arc;
use std::sync::Mutex;
use self::brain::Brain;
use self::physics::Physics;
use self::physics::PhysicsInstance;
use com::ComInstance;

use config_manager::config::ConfigManagerInstance;

pub type SimulationInstance = Arc<Mutex<Simulation>>;

pub struct Simulation {
    config: ConfigManagerInstance,
    com: ComInstance,
    physics: PhysicsInstance,
}

impl Simulation {
    fn new(config: ConfigManagerInstance, com: ComInstance) -> Simulation {
         Simulation {
             config: config.clone(),
             com,
             physics: Physics::new(config),
         }
    } 

    fn start(&self) {
        let boards = &self.config.lock().unwrap().com.boards;
        for b in boards {
            match &b.simu {
                Some(p) => {
                    let brain = Brain::new(self.com.clone(), p.to_owned());
                    Brain::start(brain.clone());
                    self.physics.lock().unwrap().add_brain(brain);
                },
                _ => {},
            }
        }
    }
}

pub fn init(config: ConfigManagerInstance, com: ComInstance) {
    let simu = Simulation::new(config, com);
    simu.start();
}
