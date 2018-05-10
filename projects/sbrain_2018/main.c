#include <cocobot/encoders.h>
#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "meca_bee.h"
#include "meca_water.h"
#include "strat_water.h"
#include "strat_bee.h"
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
      int i;
      for (i = 0; i < 8 * 3; i++)
      {
        cocobot_shifters_set(i, i % 2);
      }
      cocobot_shifters_update();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      platform_led_toggle(PLATFORM_LED0);
      platform_led_toggle(PLATFORM_LED1);

      for (i = 0; i < 8 * 3; i++)
      {
        cocobot_shifters_set(i, (i + 1) % 2);
      }
      cocobot_shifters_update();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      platform_led_toggle(PLATFORM_LED0);
      platform_led_toggle(PLATFORM_LED2);


      //disable everything
      cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

    }
  }
#endif

  //blink for the fun
  int i;
  for(i = 0; i < 20; i += 1)
  {
    platform_gpio_toggle(PLATFORM_GPIO0);

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  //int32_t zero = cocobot_position_get_distance();
  while(1)
  {
    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
    //cocobot_game_state_set_score((int)cocobot_position_get_distance() - zero);
    cocobot_game_state_display_score();

    //platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_DC0);
    //platform_motor_set_dc0_duty_cycle(0x8000);
  }
}

void run_homologation(void * arg)
{

    meca_bee_init();
    meca_water_init();

    (void)arg;
    cocobot_game_state_wait_for_starter_removed();

    //cocobot_trajectory_goto_d(200, 5000);
    //cocobot_trajectory_wait();
    //cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_0);
    //cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_1);

   // strat_domotique_register();
    //cocobot_action_scheduler_start();

    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void run_strategy(void * arg)
{
  (void)arg;

  meca_bee_init();
  meca_water_init();

  //strat_water_register();
  strat_bee_register();

  cocobot_game_state_wait_for_starter_removed();
  cocobot_game_state_add_points_to_score(5);

  //cocobot_trajectory_goto_d(100, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //cocobot_trajectory_wait();
  cocobot_action_scheduler_start();

  //cocobot_trajectory_goto_d(00, 1000);

  //while(0)
  //{
  //  cocobot_trajectory_goto_d(-5000, -1);
  //  cocobot_trajectory_wait();

  //  vTaskDelay(5000 / portTICK_PERIOD_MS);
  //}

  //cocobot_pathfinder_set_start_zone_allowed();
  //cocobot_pathfinder_execute(cocobot_position_get_x(), cocobot_position_get_y(), -200, -599, COCOBOT_PATHFINDER_MODE_EXECUTE_TRAJ_FORWARD);
  //cocobot_trajectory_goto_d(500, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
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
      ;
  //cocobot_action_scheduler_start();
}

void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  switch(pid)
  {
    case COCOBOT_COM_SET_SERVO_PID:
      {
        uint8_t id;
        int32_t value;
        uint32_t offset = 0;

        offset += cocobot_com_read_B(data, len, offset, &id);
        offset += cocobot_com_read_D(data, len, offset, &value);
        platform_servo_set_value(id, value);
      }
      break;
  }

  com_bee_handler(pid, data, len);
  com_water_handler(pid, data, len);
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
  cocobot_shifters_init();
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
          cocobot_position_set_x(-1240);
          cocobot_position_set_y(467);
          cocobot_position_set_angle(-90);
          break;

      case COCOBOT_GAME_STATE_COLOR_POS:
          cocobot_position_set_x(1240);
          cocobot_position_set_y(467);
          cocobot_position_set_angle(-90);
          break;
  }
      
  //xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );
  xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
