#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include <pcm9685.h>
#include "meca_umbrella.h"
#include "meca_seashell.h"
#include "meca_fish.h"
#include "strat_hut.h"
#include "strat_shell.h"

static unsigned int _shell_configuration;

void update_lcd(void * arg)
{
  (void)arg;

  while(1)
  {
    //update lcd
    cocobot_lcd_clear();
    
    //draw test text
    cocobot_lcd_print(0, 20, "d: %ld mm", (int32_t)cocobot_position_get_distance());
    cocobot_lcd_print(0, 35, "a: %ld deg", (int32_t)cocobot_position_get_angle());

    cocobot_lcd_render();

    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);

    platform_led_toggle(PLATFORM_LED0);
  }
}

void run_strategy(void * arg)
{
  (void)arg;
  meca_umbrella_init();
  meca_seashell_init();
  meca_fish_init();

  strat_shell_register();
  strat_hut_register();

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
    cocobot_console_send_answer("Robot secondaire");
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
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_umbrella_console_handler);
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_seashell_console_handler);
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_fish_console_handler);
  return handled;
}

void funny_action(void)
{
  meca_umbrella_open();
}

int main(void) 
{
  platform_init();
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_opponent_detection_init(3);
  cocobot_game_state_init(funny_action);

#ifdef AUSBEE_SIM
  //random shell config in simu
  _shell_configuration = rand() % 6; 
#else
  //TODO: impl me
  _shell_configuration = 0;
#endif
  cocobot_game_state_set_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION, &_shell_configuration); 

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1300);
      cocobot_position_set_y(-20);
      cocobot_position_set_angle(-90);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1300);
      cocobot_position_set_y(-20);
      cocobot_position_set_angle(-90);
      break;
  }

  xTaskCreate(run_strategy, "strat", 200, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
