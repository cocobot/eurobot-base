#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm_action.h"

int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  cocobot_arm_action_init();
  cocobot_arm_action_test_print();

  vTaskStartScheduler();

  return 0;
}
