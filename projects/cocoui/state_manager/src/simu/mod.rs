mod brain;
mod physics;

use self::brain::Brain;
use self::physics::Physics;
use self::physics::PhysicsInstance;
use crate::canars::CANFrame;
use crate::com::Com;
use crossbeam_channel::Receiver;

use config_manager::config::ConfigManagerInstance;

pub struct Simulation {
    config: ConfigManagerInstance,
    com: Com,
    physics: PhysicsInstance,
}

impl Simulation {
    fn new(config: ConfigManagerInstance, com: Com, tx_can: Receiver<CANFrame>) -> Simulation {
        Simulation {
            config: config.clone(),
            com,
            physics: Physics::new(config, tx_can),
        }
    }

    fn start(&self) {
        let boards = &self.config.lock().unwrap().com.boards;
        for b in boards {
            match &b.simu {
                Some(p) => {
                    let brain = Brain::new(self.com.clone(), p.to_owned());
                    Brain::start(brain.clone(), self.physics.clone());
                    self.physics.lock().unwrap().add_brain(brain);
                }
                _ => {}
            }
        }
    }
}

pub fn init(config: ConfigManagerInstance, com: Com, tx_can: Receiver<CANFrame>) {
    let simu = Simulation::new(config, com, tx_can);
    simu.start();
}
