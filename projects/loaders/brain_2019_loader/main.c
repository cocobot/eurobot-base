#include <platform.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>

//static void event(void)
//{
//  platform_led_toggle(PLATFORM_LED0);
//}


//void dummyFunction(void) __attribute__((used));
//
//// Never called.
//void dummyFunction(void) {
//    vTaskSwitchContext();
//}

int main(void) 
{
  platform_init();

  cocobot_can_init();
  cocobot_loader_init();

  vTaskStartScheduler();

  //DO NOT REMOVE:
  //This will never be called outside "FreeRTOS port" asm block.
  //gcc lto do not support asm and will drop the function if not used
  //outside an asm block.
  vTaskSwitchContext();
  //END: DO NOT REMOVE

  return 0;
}
