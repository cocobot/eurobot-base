use robot::Robot;

pub struct State {
    pr: Option<Robot>,
    sr: Option<Robot>,
}

impl State {
     pub fn new() -> State {
         State {
             pr: None,
             sr: None,
         }
     }
}
