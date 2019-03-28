#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();
 // cocobot_position_init(4);

  vTaskStartScheduler();

  return 0;
}
