#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "meca_sucker.h"
#include "meca_umbrella.h"
#include "meca_crimp.h"
#include "strat_hut.h"
#include "strat_little_dune.h"
#include "strat_easy_dune.h"
#include "strat_dune_attack.h"

void update_led(void * arg)
{
  (void)arg;

//#ifndef AUSBEE_SIM
//  int vbat = platform_adc_get_mV(PLATFORM_ADC_VBAT);
//  if(vbat < COCOBOT_LOW_BAT_THRESHOLD)
//  {
//    while(1)
//    {
//      platform_gpio_toggle(PLATFORM_GPIO0);
//
//      //disable everything
//      meca_sucker_disable();
//      meca_umbrella_disable();
//      meca_crimp_disable();
//      cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
//
//      vTaskDelay(500 / portTICK_PERIOD_MS);
//    }
//  }
//#endif

  while(1)
  {
    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
  }
}

void run_strategy(void * arg)
{
  (void)arg;

  meca_sucker_init();
  meca_umbrella_init();
  meca_crimp_init();

  strat_hut_register();
 // strat_little_dune_register();
  strat_easy_dune_register();
  strat_dune_attack_register();

  cocobot_game_state_wait_for_starter_removed();


  while(0)
  {
    cocobot_trajectory_goto_d(-5000, -1);
    cocobot_trajectory_wait();

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }

  cocobot_trajectory_goto_d(-100, -1);
  cocobot_trajectory_wait();

  cocobot_action_scheduler_start();
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

    cocobot_console_send_answer("%ld", cocobot_game_state_get_elapsed_time() / 1000);
    return 1;
  }

  int handled = 0;
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_umbrella_console_handler);
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_sucker_console_handler);
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_crimp_console_handler);
  return handled;
}

int main(void) 
{
  platform_init();
  /*
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_opponent_detection_init(3);
  cocobot_game_state_init(funny_action);
  cocobot_pathfinder_init(300, 220);

  //Main robot do not need to know the shell config
  _shell_configuration = 0;
  cocobot_game_state_set_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION, &_shell_configuration); 

  do_funny_action = 0;
  
  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1300);
      cocobot_position_set_y(250);
      cocobot_position_set_angle(180);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1300);
      cocobot_position_set_y(250);
      cocobot_position_set_angle(0);
      break;
  }

  xTaskCreate(run_strategy, "strat", 400, NULL, 2, NULL );
  */
  xTaskCreate(update_led, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
