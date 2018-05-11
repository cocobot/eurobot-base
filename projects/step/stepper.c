#include <platform.h>
#include <avr/interrupt.h>
#include "stepper.h"

typedef struct
{
  uint8_t home;
  int32_t position;
  int32_t target;
  int32_t reset_step;
} stepper_state_t;

static stepper_state_t _state[4];
static volatile uint8_t busy = 0;

static void stepper_enable(void)
{
  platform_gpio_set(PLATFORM_STEPPER_RESET);
  platform_gpio_set(PLATFORM_STEPPER_SLEEP);
  platform_gpio_clear(PLATFORM_STEPPER_ENABLE);
}

static void stepper_disable(void)
{
  platform_gpio_set(PLATFORM_STEPPER_ENABLE);
  platform_gpio_clear(PLATFORM_STEPPER_RESET);
  platform_gpio_clear(PLATFORM_STEPPER_SLEEP);
}

void stepper_target(int id, int32_t t)
{
  busy = 1;
  _state[id].target = t;
  busy = 1;
}

void stepper_home(int id)
{
  busy = 1;
  _state[id].home = 1;
  busy = 1;
  _state[id].target = _state[id].position - _state[id].reset_step;
  busy = 1;
}

ISR(TCD1_OVF_vect)
{
  uint32_t toggle = 0;
  int i;
  for(i = 0; i < 4; i += 1)
  {
    if(_state[i].position != _state[i].target)
    {
      switch(i)
      {
        case 0:
          toggle |= PLATFORM_STEPPER_M0_STEP;
          break;
        case 1:
          toggle |= PLATFORM_STEPPER_M1_STEP;
          break;
        case 2:
          toggle |= PLATFORM_STEPPER_M2_STEP;
          break;
        case 3:
          toggle |= PLATFORM_STEPPER_M3_STEP;
          break;
      }
      if(_state[i].target > _state[i].position)
      {
        switch(i)
        {
          case 0:
            platform_gpio_set(PLATFORM_STEPPER_M0_DIR);
            break;
          case 1:
            platform_gpio_set(PLATFORM_STEPPER_M1_DIR);
            break;
          case 2:
            platform_gpio_set(PLATFORM_STEPPER_M2_DIR);
            break;
          case 3:
            platform_gpio_set(PLATFORM_STEPPER_M3_DIR);
            break;
        }
        _state[i].position += 1;
      }
      else
      {
        switch(i)
        {
          case 0:
            platform_gpio_clear(PLATFORM_STEPPER_M0_DIR);
            break;
          case 1:
            platform_gpio_clear(PLATFORM_STEPPER_M1_DIR);
            break;
          case 2:
            platform_gpio_clear(PLATFORM_STEPPER_M2_DIR);
            break;
          case 3:
            platform_gpio_clear(PLATFORM_STEPPER_M3_DIR);
            break;
        }
        _state[i].position -= 1;
      }
    }
  }
  platform_gpio_toggle(toggle);
  if(toggle)
  {
    platform_led_toggle(PLATFORM_LED0);
    busy = 1;
  }
  else
  {
    busy = 0;
  }

  for(i = 0; i < 4; i += 1)
  {
    if(_state[i].home)
    {
      if(_state[i].position == _state[i].target)
      {
        _state[i].home = 0;
        _state[i].position = 0;
        _state[i].target = 5;
        if(_state[i].reset_step > 0)
        {
          _state[i].target = -5;
        }
      }
    }
  }
}

void stepper_init(void) 
{
  int i;

  for(i = 0; i < 4; i += 1)
  {
    _state[i].home = 0;
    _state[i].position = 0;
    _state[i].target = 0;
  }

  _state[0].reset_step = -600; //claw right
  _state[1].reset_step = 650; //claw left
  _state[2].reset_step = -2000;
  _state[3].reset_step = 3500;

  platform_gpio_clear(PLATFORM_STEPPER_MS1);
  platform_gpio_clear(PLATFORM_STEPPER_MS2);
  platform_gpio_clear(PLATFORM_STEPPER_MS3);

  stepper_disable();
  stepper_enable();

  TCD1.CTRLA = TC_CLKSEL_DIV64_gc;
  TCD1.CTRLB = TC_WGMODE_SS_gc;
  TCD1.PER = 500;
  TCD1.INTCTRLA = TC_OVFINTLVL_LO_gc;
  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
  sei();
}

int stepper_is_busy(void)
{
  return busy;
}
