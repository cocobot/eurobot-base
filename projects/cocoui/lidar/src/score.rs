use rppal::gpio::Gpio;
use rppal::gpio::OutputPin;
use std::{thread, time};

pub struct Score {
	latch: OutputPin,
	sck: OutputPin,
	mosi: OutputPin,
	score: usize,
}

impl Score {
	const DIGITS: [u8; 10] = [0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x67];

	pub fn new() -> Score {
		let mut latch = Gpio::new().unwrap().get(22).unwrap().into_output();
		let mut sck = Gpio::new().unwrap().get(11).unwrap().into_output();
		let mut mosi = Gpio::new().unwrap().get(9).unwrap().into_output();

		latch.set_low();
		sck.set_low();
		mosi.set_high();

		Score {
			latch,
			sck,
			mosi,
			score: 0,
		}
	}

	pub fn set_score(&mut self, score: usize) {
		self.score = score;
		self.display();
	}

	fn display(&mut self) {
		let mut modulo = 10;
		for _j in 0..3 {
			let dig = (self.score % modulo) / (modulo / 10);
			for i in 0..8 {
				if Score::DIGITS[dig] & (1 << (7 - i)) != 0 {
					self.mosi.set_low();
				}
				else {
					self.mosi.set_high();
				}
				self.sck.set_high();
				self.sck.set_low();
			}
			modulo = modulo * 10;
		}
		self.latch.set_high();
		self.latch.set_low();
	}
}
