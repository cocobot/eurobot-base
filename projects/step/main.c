#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>

void update_led(void * arg)
{
  (void)arg;

  while(1)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
  }
}

int main(void) 
{
  platform_init();

  xTaskCreate(update_led, "blink", 200, NULL, 1, NULL);

  vTaskStartScheduler();

  return 0;
}
