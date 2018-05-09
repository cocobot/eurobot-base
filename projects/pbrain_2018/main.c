#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "cocobot/encoders.h"
#include "strat_domotique.h"
//#include "cocobot_pathfinder_config.h"

//static unsigned int _shell_configuration;
extern cocobot_pathfinder_table_init_s initTable [];
extern cocobot_opponent_detection_fake_robot_t _fakebot;

void update_lcd(void * arg)
{
  (void)arg;

  platform_gpio_set_direction(PLATFORM_GPIO0, MCUAL_GPIO_OUTPUT);

#ifndef AUSBEE_SIM
  int vbat = platform_adc_get_mV(PLATFORM_ADC_VBAT);
  if(vbat < COCOBOT_LOW_BAT_THRESHOLD)
  {
    while(1)
    {
      platform_gpio_toggle(PLATFORM_GPIO0);

      //disable everything
      cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
#endif

#ifdef AUSBEE_SIM
  //while(1)
  //{
  //    cocobot_com_printf(".");
  //    vTaskDelay(2000/portTICK_PERIOD_MS);
  //}
#endif
  //blink for the fun
  int i;
  for(i = 0; i < 20; i += 1)
  {
    platform_gpio_toggle(PLATFORM_GPIO0);

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  while(1)
  {
    i += 1;

    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
    cocobot_game_state_display_score();
  }
}

void run_homologation(void * arg)
{
    (void)arg;
    cocobot_game_state_wait_for_starter_removed();

    cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_0);
    cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_1);

    strat_domotique_register();
    cocobot_action_scheduler_start();

    while(1)
        vTaskDelay(100/portTICK_PERIOD_MS);
}

void run_strategy(void * arg)
{
  (void)arg;

  cocobot_game_state_wait_for_starter_removed();
  //int32_t motEnc[2] = {0};

  //cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
  //cocobot_trajectory_goto_d(100, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //while(1)
  //{
  //    cocobot_encoders_get_motor_position(motEnc);
  //    cocobot_com_printf("Enc 0: %d, Enc 1: %d", motEnc[0], motEnc[1]);
  //    vTaskDelay(500/portTICK_PERIOD_MS);
  //}

  //while(1)
  //{
  //    cocobot_trajectory_goto_a(90, 10000);
  //    cocobot_trajectory_goto_a(180, 10000);
  //    cocobot_trajectory_goto_a(-90, 10000);
  //    cocobot_trajectory_goto_a(0, 10000);
  //    cocobot_trajectory_wait();
  //    cocobot_trajectory_goto_a(-90, 10000);
  //    cocobot_trajectory_goto_a(180, 10000);
  //    cocobot_trajectory_goto_a(90, 10000);
  //    cocobot_trajectory_goto_a(0, 10000);
  //    cocobot_trajectory_wait();
  //}

  //cocobot_trajectory_goto_d(1200, 7000);

  //while(1)
  //{
      cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), 1000, -600, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
      cocobot_trajectory_wait();
     // cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), 1000, 600, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
      //cocobot_trajectory_wait();
  //}
  while(0)
  {
      cocobot_trajectory_goto_d(200, 5000);
      cocobot_trajectory_goto_a(90, 10000);
      cocobot_trajectory_wait();
      cocobot_trajectory_goto_d(200, 5000);
      cocobot_trajectory_goto_a(180, 10000);
      cocobot_trajectory_wait();
      cocobot_trajectory_goto_d(200, 5000);
      cocobot_trajectory_goto_a(-90, 10000);
      cocobot_trajectory_wait();
      cocobot_trajectory_goto_d(200, 5000);
      cocobot_trajectory_goto_a(0, 10000);
      cocobot_trajectory_wait();
  }
  //while(1);
  //while(0)
  //{
  //  cocobot_trajectory_goto_d(-5000, -1);
  //  cocobot_trajectory_wait();

  //  vTaskDelay(5000 / portTICK_PERIOD_MS);
  //}

  //cocobot_pathfinder_set_start_zone_allowed();
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), 125, 525, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_wait();
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), -1000, -500, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_wait();
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), 1100, -550, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_wait();
  //_fakebot.x = -200;
  //_fakebot.y = 200;
  //_fakebot.activated = COCOBOT_OPPONENT_DETECTION_ACTIVATED;
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), -200, -300, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_wait();
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), -500, -200, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_wait();

  while(1)
      vTaskDelay(100/portTICK_PERIOD_MS);
  //cocobot_action_scheduler_start();
 }

void com_handler(uint16_t pid)
{
#if 0
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
#else
  (void)pid;
#endif
}

int main(int argc, char *argv[]) 
{
#ifdef AUSBEE_SIM
  mcual_arch_main(argc, argv);
#else
  (void)argc;
  (void)argv;
#endif
  platform_init();
  cocobot_com_init(MCUAL_USART1, 1, 1, com_handler);
  cocobot_lcd_init();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_opponent_detection_init(3);
  cocobot_game_state_init(NULL);
  cocobot_pathfinder_init(initTable);
  cocobot_action_scheduler_use_pathfinder(1);

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
  cocobot_position_set_x(-1280);
      cocobot_position_set_y(750);
      cocobot_position_set_angle(-90);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1280);
      cocobot_position_set_y(750);
      cocobot_position_set_angle(-90);
      break;
  }
      
  //xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );
  xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
