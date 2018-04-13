#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>


int main(void) 
{
  platform_init();
  vTaskStartScheduler();

  return 0;
}
