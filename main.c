#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
//#include <position.h>
//#include <asserv.h>
//#include <pcm9685.h>
//#include <max11628.h>
//#include <max7317.h>
//#include <gyro.h>
//#include <trajectory.h>
//#include <usir.h>
//#include <step.h>
//#include <pathfinder.h>
//#include "meca.h"
//#include "strat.h"

void blink(void * arg)
{
  (void)arg;
  int test = 0;

  platform_led_clear(PLATFORM_LED2);
  vTaskDelay(2000 / portTICK_PERIOD_MS); 
  while(1)
  {
    platform_led_set(PLATFORM_LED2);
    cocobot_trajectory_goto_d(50, -1);
    cocobot_trajectory_goto_d(100, -1);
    cocobot_trajectory_goto_d(-70, -1);
    cocobot_trajectory_goto_d(-80, -1);
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }

  //platform_gpio_set(PLATFORM_GPIO_MOTOR_ENABLE);
  //platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_RIGHT);

  //platform_motor_set_left_duty_cycle(0x750);
  //platform_motor_set_right_duty_cycle(0x750);
  while(1)
  {
    //update lcd
    cocobot_lcd_clear();
    
    //draw test line
    cocobot_lcd_draw_line(COCOBOT_LCD_X_MAX / 2, 0, COCOBOT_LCD_X_MAX / 2, COCOBOT_LCD_Y_MAX - 1);

    //draw test text
    cocobot_lcd_print(12, 5, "Cocobot %u", test++);
   // cocobot_lcd_print(0, 20, "d: %ld mm", (int32_t)cocobot_position_get_distance());
    cocobot_lcd_print(0, 20, "a: %lX deg", (uint32_t)cocobot_position_get_angle());
    cocobot_lcd_print(0, 35, "a: %ld deg", (int32_t)cocobot_position_get_angle());

    cocobot_lcd_render();

    //toggle led
    platform_led_toggle(PLATFORM_LED1 | PLATFORM_LED0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

int console_handler(const char * cmd)
{
  (void)cmd;
  return 0;
}

int main(void) 
{
  platform_init();
  cocobot_lcd_init();
  cocobot_position_init(3);
  cocobot_asserv_init();
  platform_led_clear(PLATFORM_LED2);
  cocobot_trajectory_init(3);
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);

  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);

  xTaskCreate(blink, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
