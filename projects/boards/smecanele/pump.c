#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <cocobot.h>
#include <platform.h>

#define PUMP_1  PLATFORM_GPIO_PUMP1
#define PUMP_2  PLATFORM_GPIO_PUMP2
#define EMPTY_SOL_1  PLATFORM_GPIO_VALVE1
#define EMPTY_SOL_2  PLATFORM_GPIO_VALVE2

uint8_t volatile _sucker_status[4];

static void pump_thread(void * arg)
{
  uint8_t * val = arg; 
  while(1)
  {
    switch(_sucker_status[0]) 
    {
      case 0:
      case 3:
        platform_gpio_set(EMPTY_SOL_1);
        platform_gpio_clear(PUMP_1);
        _sucker_status[0] = 0;
        break;

      case 1:
      case 2:
        platform_gpio_set(PUMP_1);
        platform_gpio_clear(EMPTY_SOL_1);
        _sucker_status[0] = 0;
        break;
    }

    switch(_sucker_status[1]) 
    {
      case 0:
      case 3:
        platform_gpio_set(EMPTY_SOL_2);
        platform_gpio_clear(PUMP_2);
        _sucker_status[1] = 0;
        break;

      case 1:
      case 2:
        platform_gpio_set(PUMP_2);
        platform_gpio_clear(EMPTY_SOL_2);
        _sucker_status[1] = 0;
        break;
    }

    vTaskDelay(25/portTICK_PERIOD_MS);
  }
}

void pump_init(void)
{
  int i;
  for(i = 0; i < 2; i += 1)
  {
    _sucker_status[i] = 0;
  }
 
  xTaskCreate(pump_thread, "pump", 1024, (void *)0, 3, NULL);
}

void pump_set_state(uint8_t pump_id, uint8_t action)
{
  _sucker_status[pump_id] = action;
}

uint8_t pump_get_state(uint8_t pump_id)
{
  return _sucker_status[pump_id];
}
