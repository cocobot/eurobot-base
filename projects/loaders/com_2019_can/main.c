#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>

 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  vTaskStartScheduler();

  return 0;
}
