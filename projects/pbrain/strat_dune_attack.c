#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include "strat_dune_attack.h"

#define QUIT_IF_FAILED() { if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS) { return COCOBOT_RETURN_ACTION_SUCCESS;}}

static float strat_dune_attack_get_x(void)
{
  float target = 0;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_POS)
  {
    return 0;
  }
  return target;
}

static float strat_dune_attack_get_y(void)
{
  return 550;
}

static float strat_dune_attack_get_a(void)
{
  return 90;
}

float strat_dune_attack_get_score(void)
{
  return 14;
}

float strat_dune_attack_get_exec_time(void)
{
  return 5000;
}

float strat_dune_attack_get_success_proba(void)
{
  return 0.20;
}

static cocobot_action_callback_result_t strat_dune_attack_preexec(void * arg)
{
  (void)arg;

  cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);
  cocobot_action_scheduler_use_pathfinder(0);

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_dune_attack_cleanup(void * arg)
{
  (void)arg;
  cocobot_action_scheduler_use_pathfinder(0);

  cocobot_trajectory_goto_d(-500, 2000);
  QUIT_IF_FAILED();



  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_dune_attack_exec(void * arg)
{
  (void)arg;

  int x_k = 1;
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    x_k = -1;
  }

  (void)x_k;

  cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);

  cocobot_trajectory_goto_a(90, 1000);
  cocobot_trajectory_wait();

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  cocobot_trajectory_goto_xy(strat_dune_attack_get_x(), 620, 7000);
  cocobot_trajectory_wait();

#ifdef AUSBEE_SIM
  int try = 5;
#endif
  while(1)
  {
    cocobot_trajectory_goto_d(20, 1000);
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
  cocobot_trajectory_goto_d(-10, 1000);
  cocobot_trajectory_wait();

  
  vTaskDelay(1500 / portTICK_PERIOD_MS);

  cocobot_asserv_slow();
  cocobot_trajectory_goto_d(-100, 1500);
  cocobot_trajectory_wait();

  cocobot_trajectory_goto_d(30, 1000);
  cocobot_trajectory_wait();

  vTaskDelay(2500 / portTICK_PERIOD_MS);

  cocobot_trajectory_goto_d(60, 3000);
  cocobot_trajectory_wait();

  vTaskDelay(2500 / portTICK_PERIOD_MS);

  cocobot_trajectory_goto_d(-70, 1000);
  QUIT_IF_FAILED();


  cocobot_trajectory_goto_xy(x_k * 1050, strat_dune_attack_get_y(), 10000);
  QUIT_IF_FAILED();

  cocobot_trajectory_goto_xy(x_k * 1000, -120, 10000);
  QUIT_IF_FAILED();



  cocobot_trajectory_goto_xy(x_k * 300, -120, 10000);
  QUIT_IF_FAILED();

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_a(10, 2000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_a(170, 2000);
      break;
  }

  QUIT_IF_FAILED();
  cocobot_trajectory_goto_d(50, 2000);
  QUIT_IF_FAILED();

   
  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static void strat_dune_attack_pos(void *arg, float *x, float *y, float *a)
{
  (void)arg;
  *x = strat_dune_attack_get_x();
  *y = strat_dune_attack_get_y();
  *a = strat_dune_attack_get_a();
}

void strat_dune_attack_register(void)
{
  cocobot_action_scheduler_add_action(
                                    "dune_attack",
                                    strat_dune_attack_get_score(),
                                    strat_dune_attack_pos,
                                    strat_dune_attack_get_exec_time(),
                                    strat_dune_attack_get_success_proba(),
                                    strat_dune_attack_preexec,
                                    strat_dune_attack_exec,
                                    strat_dune_attack_cleanup,
                                    NULL,
                                    NULL);
}
