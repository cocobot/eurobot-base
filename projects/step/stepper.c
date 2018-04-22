#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "stepper.h"

typedef struct
{
  uint8_t home;
  int32_t position;
  int32_t target;
  int32_t reset_step;
} stepper_state_t;

static stepper_state_t _state[4];
static SemaphoreHandle_t _xSemaphore;

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

static void stepper_home(int id)
{
  _state[id].home = 1;
  _state[id].target = _state[id].position - _state[id].reset_step;
  xSemaphoreGive(_xSemaphore);
}

void run_stepper(void * arg)
{
  (void)arg;

  stepper_disable();
  stepper_enable();

  while(1)
  {
    int busy = 1;
    while(busy)
    {
      busy = 0;
      vTaskDelay(1 / portTICK_PERIOD_MS);
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
          }
        }
      }
      if(toggle)
      {
        busy = 1;
      }
      platform_gpio_toggle(toggle);

      for(i = 0; i < 4; i += 1)
      {
        if(_state[i].home)
        {
          if(_state[i].position == _state[i].target)
          {
            _state[i].home = 0;
          }
        }
      }
    }

    xSemaphoreTake(_xSemaphore, portMAX_DELAY);
  }
}

void update_stepper(void * arg)
{
  (void)arg;

  while(1)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
  }
}

void stepper_init(void) 
{
  int i;

  _xSemaphore = xSemaphoreCreateBinary();

  for(i = 0; i < 4; i += 1)
  {
    _state[i].home = 0;
    _state[i].position = 0;
    _state[i].target = 0;
  }

  _state[0].reset_step = -200;
  _state[1].reset_step = -200;
  _state[2].reset_step = -200;
  _state[3].reset_step = -200;

  for(i = 0; i < 4; i += 1)
  {
    stepper_home(i);
  }

  platform_gpio_set(PLATFORM_STEPPER_MS1);
  platform_gpio_clear(PLATFORM_STEPPER_MS2);
  platform_gpio_clear(PLATFORM_STEPPER_MS3);

  xTaskCreate(run_stepper, "rstep", 200, NULL, 2, NULL);
  xTaskCreate(update_stepper, "ustep", 200, NULL, 2, NULL);
}
