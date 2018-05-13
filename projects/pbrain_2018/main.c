#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include "cocobot/encoders.h"
#include "strat_domotique.h"
#include "meca.h"
//#include "cocobot_pathfinder_config.h"

//static unsigned int _shell_configuration;
extern cocobot_pathfinder_table_init_s initTable [];
extern cocobot_opponent_detection_fake_robot_t _fakebot;
uint8_t no_bat = 1;

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
      no_bat = 1;
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



      platform_gpio_toggle(PLATFORM_GPIO0);

      //disable everything
      cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
    }
  }
#endif
  no_bat = 0;

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
     meca_init();

    (void)arg;
    cocobot_game_state_wait_for_starter_removed();

    meca_take();

    //panneau domotique
    cocobot_game_state_add_points_to_score(5);
    float x = 325; 
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        x = -x;
    }
    float y = 800; 
    cocobot_trajectory_goto_xy(x, y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();

    cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();


    #ifdef AUSBEE_SIM
  int try = 5;
#endif
  while(1)
  {
      cocobot_trajectory_goto_d(-20, 1000);
      if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
      {
          break;
      }
#ifdef AUSBEE_SIM
      try -= 1;
      if(try == 0)
      {
          break;
      }
#endif
  }

  cocobot_game_state_add_points_to_score(25);


  
  int i = 0;
  for(i = 0; i < 3; i += 1)
  {
    cocobot_trajectory_goto_a(-95, 1000);
    cocobot_trajectory_wait();
    cocobot_trajectory_goto_a(-85, 1000);
    cocobot_trajectory_wait();
  }
  cocobot_trajectory_goto_a(-90, 2000);
  cocobot_trajectory_wait();

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  
  //try 3
  cocobot_trajectory_goto_d(10, 5000);
  cocobot_trajectory_wait();
  cocobot_trajectory_goto_a(-90, 2000);
  cocobot_trajectory_wait();


  #ifdef AUSBEE_SIM
  try = 5;
#endif
  while(1)
  {
      cocobot_trajectory_goto_d(-20, 1000);
      if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
      {
          break;
      }
#ifdef AUSBEE_SIM
      try -= 1;
      if(try == 0)
      {
          break;
      }
#endif
  }

  cocobot_trajectory_goto_a(-90, 2000);
  cocobot_trajectory_wait();
  cocobot_asserv_set_angle_activation(0);
  cocobot_trajectory_goto_d(-20, 1500);
  cocobot_asserv_set_angle_activation(1);



  cocobot_trajectory_goto_d(20, 2000);

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  #ifdef AUSBEE_SIM
  try = 5;
#endif
  while(1)
  {
      cocobot_trajectory_goto_d(-20, 1000);
      if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
      {
          break;
      }
#ifdef AUSBEE_SIM
      try -= 1;
      if(try == 0)
      {
          break;
      }
#endif
  }


  cocobot_trajectory_goto_d(100, 5000);
  cocobot_trajectory_wait();


  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    cocobot_trajectory_goto_a(180, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  }
  else
  {
    cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  }
  cocobot_trajectory_wait();

  cocobot_trajectory_goto_d(150, 5000);
  cocobot_trajectory_wait();

  cocobot_game_state_add_points_to_score(3);
  meca_prepare();

  cocobot_trajectory_goto_d(-150, 5000);
  cocobot_trajectory_wait();


  cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  cocobot_trajectory_wait();

  cocobot_trajectory_goto_d(100, 5000);
  cocobot_trajectory_wait();



  //cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_0);
    //cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_1);

   // strat_domotique_register();
   //cocobot_action_scheduler_start();

  vTaskDelay(100 / portTICK_PERIOD_MS);

  while(cocobot_game_state_get_elapsed_time() < 75000)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }


  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    cocobot_trajectory_goto_a(180 + 35, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  }
  else
  {
    cocobot_trajectory_goto_a(-35, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  }
  cocobot_trajectory_wait();

  //meca
  meca_prepare(); 

  cocobot_trajectory_goto_d(400, 5000);
  cocobot_trajectory_wait();

  cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  cocobot_trajectory_wait();


  cocobot_trajectory_goto_d(250, 5000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(2); //
#ifdef AUSBEE_SIM
  try = 5;
#endif
  while(1)
  {
      cocobot_trajectory_goto_d(10, 1000);
      if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
      {
          break;
      }
#ifdef AUSBEE_SIM
      try -= 1;
      if(try == 0)
      {
          break;
      }
#endif
  }
  cocobot_trajectory_goto_d(-20, 1000);
  cocobot_trajectory_wait();


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

void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  if(!no_bat)
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
    meca_com_handler(pid, data, len);
  }
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
      cocobot_position_set_x(-1225 - 60);
      cocobot_position_set_y(800);
      cocobot_position_set_angle(0);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1225 + 60);
      cocobot_position_set_y(800);
      cocobot_position_set_angle(180);
      break;
  }
      
  //xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );
  xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
