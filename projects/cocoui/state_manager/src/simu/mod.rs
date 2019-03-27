mod physics;
mod brain;

use std::sync::Arc;
use std::sync::Mutex;
use self::brain::Brain;
use self::brain::BrainInstance;
use self::physics::Physics;
use self::physics::PhysicsInstance;

use config_manager::config::ConfigManagerInstance;

pub type SimulationInstance = Arc<Mutex<Simulation>>;

pub struct Simulation {
    config: ConfigManagerInstance,
    physics: PhysicsInstance,
}

impl Simulation {
    fn new(config: ConfigManagerInstance) -> Simulation {
         Simulation {
             config: config.clone(),
             physics: Physics::new(config),
         }
    } 

    fn start(&self) {
        let boards = &self.config.lock().unwrap().com.boards;
        for b in boards {
            match &b.simu {
                Some(p) => {
                    let brain = Brain::new(p.to_owned());
                    Brain::start(brain.clone());
                    self.physics.lock().unwrap().add_brain(brain);
                },
                _ => {},
            }
        }
    }
}

pub fn init(config: ConfigManagerInstance) {
    let simu = Simulation::new(config);
    simu.start();
}
