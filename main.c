#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "meca_sucker.h"

static unsigned int _shell_configuration;

void update_lcd(void * arg)
{
  (void)arg;
  while(1)
  {
    //update lcd
    cocobot_lcd_clear();
    
   // cocobot_lcd_print(0, 20, "d: %ld mm", (int32_t)cocobot_position_get_distance());
    cocobot_lcd_print(0, 20, "a: %lX deg", (uint32_t)cocobot_position_get_angle());
    cocobot_lcd_print(0, 35, "a: %ld deg", (int32_t)cocobot_position_get_angle());

    cocobot_lcd_render();

    //toggle led
    platform_led_toggle(PLATFORM_LED1 | PLATFORM_LED0);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_CLOSE);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    //meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_PUMP);
    //vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void run_strategy(void * arg)
{
  (void)arg;

  meca_sucker_init();
  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  cocobot_trajectory_goto_xy(0, 700, -1);
  cocobot_trajectory_goto_xy_backward(0, 800, -1);
  cocobot_trajectory_goto_a(-90, -1);
  cocobot_trajectory_wait();

  meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_PUMP);
  cocobot_trajectory_goto_xy_backward(0, 1200, 1000);
  cocobot_trajectory_goto_d(700, -1);
  cocobot_trajectory_wait();

  cocobot_trajectory_wait();

  while(1);

  cocobot_game_state_wait_for_starter_removed();

  cocobot_action_scheduler_start();
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
    cocobot_console_send_answer("Robot principal");
    cocobot_console_send_answer("%ld", platform_adc_get_mV(PLATFORM_ADC_VBAT));
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
      cocobot_console_send_answer("Violet");
    }
    else
    {
      cocobot_console_send_answer("Green");
    }
    void * ptr = cocobot_game_state_get_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION);
    cocobot_console_send_answer("%d", *((unsigned int *)ptr));
    return 1;
  }

  int handled = 0;
  //COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_umbrella_console_handler);
  return handled;
}

void funny_action(void)
{

}

int main(void) 
{
  platform_init();
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(4);
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_game_state_init(funny_action);

  //Main robot do not need to know the shell config
  _shell_configuration = 0;
  cocobot_game_state_set_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION, &_shell_configuration); 
  
  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1250);
      cocobot_position_set_y(300);
      cocobot_position_set_angle(0);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1250);
      cocobot_position_set_y(300);
      cocobot_position_set_angle(180);
      break;
  }

  xTaskCreate(run_strategy, "strat", 200, NULL, 2, NULL );
  xTaskCreate(update_lcd, "lcd", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
