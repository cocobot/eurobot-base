#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>

extern cocobot_pathfinder_table_init_s initTable [];


void run_homologation(void * arg)
{
  (void)arg;
  cocobot_game_state_wait_for_starter_removed();

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


  while(1)
  {
    cocobot_trajectory_goto_d(-20, 1000);
    if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
    {
      break;
    }
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


  while(1)
  {
    cocobot_trajectory_goto_d(-20, 1000);
    if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
    {
      break;
    }
  }

  cocobot_trajectory_goto_a(-90, 2000);
  cocobot_trajectory_wait();
  cocobot_asserv_set_angle_activation(0);
  cocobot_trajectory_goto_d(-20, 1500);
  cocobot_asserv_set_angle_activation(1);



  cocobot_trajectory_goto_d(20, 2000);

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  while(1)
  {
    cocobot_trajectory_goto_d(-20, 1000);
    if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
    {
      break;
    }
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

  cocobot_trajectory_goto_d(400, 5000);
  cocobot_trajectory_wait();

  cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  cocobot_trajectory_wait();


  cocobot_trajectory_goto_d(250, 5000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(2); //
  while(1)
  {
    cocobot_trajectory_goto_d(10, 1000);
    if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
    {
      break;
    }
  }
  cocobot_trajectory_goto_d(-20, 1000);
  cocobot_trajectory_wait();


  while(1)
    vTaskDelay(100/portTICK_PERIOD_MS);
}
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  //cocobot_opponent_detection_init(3);
  cocobot_game_state_init(NULL);
  cocobot_pathfinder_init(initTable);
  cocobot_action_scheduler_use_pathfinder(1);

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1225 - 60);
      cocobot_position_set_y(600);
      cocobot_position_set_angle(0);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1225 + 60);
      cocobot_position_set_y(600);
      cocobot_position_set_angle(180);
      break;
  }

  xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );

  vTaskStartScheduler();

  return 0;
}
