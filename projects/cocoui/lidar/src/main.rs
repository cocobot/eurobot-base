use std::{thread, time};

mod score;
mod xv11;

fn main() {
    let mut score = score::Score::new();
    let mut xv11 = xv11::XV11::new();

    xv11::XV11::start(&mut xv11);

    let mut i = 0;
    loop {
        let locked_xv = xv11.lock().unwrap();
        if let Some(v) = locked_xv.get_angle(0) {
            score.set_score(v as usize);
        }
        else {
            score.set_score(999);
        }
        drop(locked_xv);

        thread::sleep(time::Duration::from_millis(100));
        i = i + 1;
    }
}

