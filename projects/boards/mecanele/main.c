#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>

void blink(void)
{
    while(1)
    {
        platform_led_toggle(PLATFORM_LED0);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  vTaskStartScheduler();

  return 0;
}
