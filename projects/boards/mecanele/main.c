#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"

int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  cocobot_arm_init();
  //cocobot_arm_test_and_print();

  vTaskStartScheduler();

  return 0;
}
