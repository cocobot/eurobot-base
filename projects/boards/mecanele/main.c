#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm_action.h"
#include "pcm9685.h"

static void blink(void * arg)
{
    while(1)
    {
        platform_led_toggle(PLATFORM_LED0);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

static void servo(void * arg)
{
    static uint32_t value = 100;
    while(1)
    {
        platform_servo_set_value(PLATFORM_SERVO_0_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_1_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_2_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_3_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_4_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_5_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_6_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_7_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_8_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_9_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_10_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_11_ID, value);
        platform_servo_set_value(PLATFORM_SERVO_12_ID, value);
        value += 10;
        if(value == 500)
            value = 100;
        vTaskDelay(100/portTICK_PERIOD_MS);
    } 
}


int main(void) 
{
  platform_init();

  //cocobot_com_init();
  //cocobot_com_run();

  //cocobot_arm_action_init();

  xTaskCreate(blink, "can", 1024, NULL, 1, NULL);
  xTaskCreate(servo, "servo", 1024, NULL, 1, NULL);
  vTaskStartScheduler();

  return 0;
}
