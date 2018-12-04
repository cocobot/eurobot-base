#include <mcual.h>
#include <platform.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>

void test(void * arg)
{
  (void)arg;

  int i;
  int color = 0;
  while(1)
  {

    for(i = 0; i < 5; i += 1)
    {
      if(color & 1)
      {
        platform_led_set(1 << i);
      }
      else
      {
        platform_led_clear(1 << i);
      }

      if(color & 2)
      {
        platform_led_set(1 << (i + 10));
      }
      else
      {
        platform_led_clear(1 << (i + 10));
      }

      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    color += 1;
  }
}
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_loader_init();

  cocobot_com_run();

  xTaskCreate(test, "test", 600, NULL, 2, NULL );
  vTaskStartScheduler();
  vTaskSwitchContext();

  return 0;
}
