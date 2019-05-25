#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm_action.h"

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

  cocobot_arm_action_init();

  vTaskStartScheduler();

  return 0;
}
