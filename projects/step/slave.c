#include <avr/io.h>
#include <platform.h>
#include <avr/interrupt.h>
#include "slave.h"
#include "stepper.h"

#define SLAVE_NO_DATA 0x42

void slave_init(void)
{
  // port
  PORTD.DIRCLR = (1 << 4) | (1 << 5) | (1 << 7);
  PORTD.DIRSET = 1 << 6;
  PORTD.PIN4CTRL = PORT_ISC_FALLING_gc;
  PORTD.INT0MASK = (1 << 4);
  PORTD.INTCTRL = PORT_INT0LVL_HI_gc;
  
  //init spi
  SPID.CTRL = SPI_ENABLE_bm | SPI_MODE_3_gc;
  SPID.INTCTRL = SPI_INTLVL_HI_gc;

  stepper_home(0); 
  stepper_home(1); 
  stepper_home(2); 
  stepper_home(3); 

}

//CS down
ISR(PORTD_INT0_vect)
{
  SPID.DATA = stepper_is_busy();
}

//SPI Data
ISR(SPID_INT_vect)
{
  static volatile uint8_t read;
  read = SPID.DATA;
  SPID.DATA = stepper_is_busy();
  switch(read)
  {
    case 0x10:
      stepper_target(0, -5); 
      break;

    case 0x11:
      stepper_target(1, 5); 
      break;

    case 0x12:
      stepper_target(1, -5); 
      break;

    case 0x13:
      stepper_target(3, 5); 
      break;

    case 0x20:
      stepper_target(0, -675); 
      break;

    case 0x21:
      stepper_target(1, 675); 
      break;

    case 0x22:
      stepper_target(2, -1000); 
      break;

    case 0x23:
      stepper_target(3, 1000); 
      break;

    case 0x32:
      stepper_target(2, -1500); 
      break;

    case 0x33:
      stepper_target(3, 1500); 
      break;

    case 0x42:
      stepper_target(3, 2100); 
      break;

    case 0x43:
      stepper_target(3, -2100); 
      break;

  }
}
