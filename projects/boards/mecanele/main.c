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

  cocobot_inverse_kinematics_init_DH_parameters(0.0971, 0.0637, 0.150, 0.080, 0.040, -0.0117);

  vTaskStartScheduler();

  return 0;
}
