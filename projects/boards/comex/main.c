#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>

static void servo(void * arg)
{
    (void)arg;
    static uint32_t value = 550;
    while(1)
    {
        platform_servo_set_value(PLATFORM_SERVO_0, value);
        platform_servo_set_value(PLATFORM_SERVO_1, value);
        value += 100;
        if(value > 2450)
            value = 550;
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
}
 
static void blink(void * arg)
{
    (void)arg;
    while(1)
    {
        platform_led_toggle(PLATFORM_LED_RED_5);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

int main(void) 
{
  platform_init();

  //Remove com from now
  //cocobot_com_init();
  //cocobot_com_run();

  xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);
  xTaskCreate(servo, "servo", 1024, NULL, 1, NULL);

  vTaskStartScheduler();
  vTaskSwitchContext();

  return 0;
}
