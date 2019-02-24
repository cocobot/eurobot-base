use std::thread;
use std::time;
use std::process::{Command, Stdio};

use config_manager::config::ConfigManagerInstance;

pub fn init(config: ConfigManagerInstance) {
    let boards = &config.lock().unwrap().com.boards;
    for b in boards {
        match &b.path {
            Some(p) => {
                warn!("Not impl {:?}", p);
                let mut child_shell = Command::new(p)
                    .stdin(Stdio::piped())
                    .stdout(Stdio::piped())
                    .stderr(Stdio::piped())
                    .spawn()
                    .unwrap();
            },
            _ => {},
        }
    }

    /*

    thread::spawn(move || {
        loop {
            warn!("Not impl");
            thread::sleep( time::Duration::from_secs(2));
        }
    });
    */
}
