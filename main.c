#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "meca_umbrella.h"

void update_lcd(void * arg)
{
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

void run_strategy(void * arg)
{
  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);
  while(1)
  {
    if(!cocobot_action_scheduler_execute_best_action())
    {
      //wait small delay if no action is available (which is a bad thing)
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
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

  int handled = 0;
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_umbrella_console_handler);
  return handled;
}

int main(void) 
{
  platform_init();
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(3);
  cocobot_asserv_init();
  cocobot_trajectory_init(3);
  cocobot_action_scheduler_init();

  meca_umbrella_init();
  

  xTaskCreate(run_strategy, "strat", 200, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
