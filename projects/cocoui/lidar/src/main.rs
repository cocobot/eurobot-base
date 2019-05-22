use rppal::spi::{Bus, Mode, SlaveSelect, Spi};
use rppal::gpio::Gpio;
use std::{thread, time};
use rppal::uart::{Parity, Uart};

mod score;

fn main() {
	let mut score = score::Score::new();

	let mut uart = Uart::new(115_200, Parity::None, 8, 1).unwrap();


 	uart.set_read_mode(1, time::Duration::default()).unwrap();

	let mut i = 0;
	loop {
		score.set_score(i);
		thread::sleep(time::Duration::from_millis(100));
		i = i + 1;
	}
}

