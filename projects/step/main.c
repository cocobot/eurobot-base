#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <mcual.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "slave.h"
#include "stepper.h"

static volatile int tick = 0;

ISR (TCC1_OVF_vect)
{
  platform_led_clear(PLATFORM_LED0);
  tick += 1;
}

int main(void) 
{
  platform_init();
  stepper_init();
  slave_init();


  TCC1.CTRLA = TC_CLKSEL_DIV256_gc;
  TCC1.CTRLB = TC_WGMODE_SS_gc;
  TCC1.PER = 50000;
  TCC1.INTCTRLA = TC_OVFINTLVL_LO_gc;
  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;


  while(1)
  {
  }

  return 0;
}
