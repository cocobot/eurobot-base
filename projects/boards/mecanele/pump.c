#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <cocobot.h>
#include <platform.h>

#define PUMP_1  PLATFORM_GPIO_PUMP2
#define PUMP_2  PLATFORM_GPIO_PUMP1
#define EMPTY_SOL_1  PLATFORM_GPIO_VALVE1
#define EMPTY_SOL_2  PLATFORM_GPIO_VALVE5

uint8_t _sucker_status[4];
uint32_t _sucker_id[4] = {
  PLATFORM_GPIO_VALVE1,
  PLATFORM_GPIO_VALVE2,
  PLATFORM_GPIO_VALVE1,
  PLATFORM_GPIO_VALVE1,
};

static void pump_check(uint8_t id)
{
  if(_sucker_status[id])
  {
    if(id < 2)
    {
      platform_gpio_set(PUMP_1);
    }
    else
    {
      platform_gpio_set(PUMP_2);
    }
  }
  else
  {
    if(id < 2)
    {
      platform_gpio_set(EMPTY_SOL_1);
    }
    else
    {
      platform_gpio_set(EMPTY_SOL_2);
    }
  }

  vTaskDelay(150/portTICK_PERIOD_MS);

  platform_gpio_set(_sucker_id[id]);

  vTaskDelay(850/portTICK_PERIOD_MS);
  platform_gpio_clear(_sucker_id[id]);
  if(id < 2)
  {
    platform_gpio_clear(PUMP_1);
  }
  else
  {
    platform_gpio_clear(PUMP_2);
  }
}

static void pump_thread(void * arg)
{
  uint8_t * val = arg; 
  uint8_t p2 = 0;

  if(val == (uint8_t *)1)
  {
    p2 = 1;
  }

  if(p2)
  {
    vTaskDelay(500/portTICK_PERIOD_MS);
  }

  while(1)
  {
    int i;
    for(i = 0; i < 2; i += 1)
    {
      if(p2)
      {
        pump_check(i + 0);
      }
      else
      {
        pump_check(i + 2);
      }
    }
  } 
}

void pump_init(void)
{
  int i;
  for(i = 0; i < 4; i += 1)
  {
    _sucker_status[i] = 0;
  }
 
  xTaskCreate(pump_thread, "pump", 1024, (void *)0, 3, NULL);
  xTaskCreate(pump_thread, "pump", 1024, (void *)1, 3, NULL);
}

void pump_set_state(uint8_t pump_id, uint8_t action)
{
  _sucker_status[pump_id] = action;
}
