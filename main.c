#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>

void blink(void * arg)
{
  (void)arg;

  platform_led_clear(PLATFORM_LED2);
  vTaskDelay(2000 / portTICK_PERIOD_MS); 
  //cocobot_asserv_set_distance_set_point(2000);
  while(0)
  {
    platform_led_toggle(PLATFORM_LED2);
    cocobot_trajectory_goto_d(500, -1);
    cocobot_trajectory_goto_a(-90, -1);
    cocobot_trajectory_goto_d(500, -1);
    cocobot_trajectory_goto_a(180, -1);
    cocobot_trajectory_goto_d(500, -1);
    cocobot_trajectory_goto_a(90, -1);
    cocobot_trajectory_goto_d(500, -1);
    cocobot_trajectory_goto_a(0, -1);
    cocobot_trajectory_wait();
  }
  while(0)
  {
    platform_led_toggle(PLATFORM_LED2);
    cocobot_trajectory_goto_d(50,  -1);
    cocobot_trajectory_goto_d(100, -1);
    cocobot_trajectory_goto_d(-70, -1);
    cocobot_trajectory_goto_d(-80, -1);
    cocobot_trajectory_wait();
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }

  //platform_gpio_set(PLATFORM_GPIO_MOTOR_ENABLE);
  //platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_RIGHT);
  //platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_LEFT);

  //platform_motor_set_left_duty_cycle(0x1000);
  //platform_motor_set_right_duty_cycle(0x1000);
  while(1)
  {
    //update lcd
    cocobot_lcd_clear();
    
    //draw test text
    cocobot_lcd_print(0, 20, "d: %ld mm", (int32_t)cocobot_position_get_distance());
    cocobot_lcd_print(0, 35, "a: %ld deg", (int32_t)cocobot_position_get_angle());

    cocobot_lcd_render();

    //toggle led
    platform_led_toggle(PLATFORM_LED1 | PLATFORM_LED0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

int console_handler(const char * command)
{
  if(strcmp(command,"info") == 0)
  {
    cocobot_console_send_answer("Robot secondaire");
    cocobot_console_send_answer("%ld", platform_adc_get_mV(PLATFORM_ADC_VBAT));
    return 1;
  }
  return 0;
}

int main(void) 
{
  platform_init();
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(3);
  cocobot_asserv_init();
  //platform_led_clear(PLATFORM_LED2);
  //cocobot_trajectory_init(3);

  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);

  xTaskCreate(blink, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
