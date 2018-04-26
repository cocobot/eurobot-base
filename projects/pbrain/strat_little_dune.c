#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include <FreeRTOS.h>
#include <task.h>
#include "strat_little_dune.h"

static float strat_little_dune_get_x(void)
{
  float target = 625;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_little_dune_get_y(void)
{
  return 470;
}

static float strat_little_dune_get_a(void)
{
  return 90;
}

static void strat_little_dune_pos(void *arg, float *x, float *y, float *a)
{
  (void)arg;
  *x = strat_little_dune_get_x();
  *y = strat_little_dune_get_y();
  *a = strat_little_dune_get_a();
}

static float strat_little_dune_get_exec_time(void)
{
  return 20000;
}

float strat_little_dune_get_success_proba(void)
{
  return 0.5;
}

float strat_little_dune_get_score(void)
{
  return 24;
}

static cocobot_action_callback_result_t strat_little_dune_preexec(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_little_dune_cleanup(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_little_dune_exec(void * arg)
{
  (void)arg;

  cocobot_trajectory_goto_xy(625, 690, 5000);
 // cocobot_trajectory_goto_d(300, 2000);
  cocobot_trajectory_wait();

  vTaskDelay(250 / portTICK_PERIOD_MS);

  vTaskDelay(3000 / portTICK_PERIOD_MS);


  vTaskDelay(1500 / portTICK_PERIOD_MS);

  cocobot_trajectory_goto_d(-400, 5000);
  cocobot_trajectory_wait();

  int x_k = 1;
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    x_k = -1;
  }

  while(1)
  {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }

  //cocobot_trajectory_goto_xy(x_k * 500, strat_little_dune_get_y(), 10000);

  //cocobot_trajectory_wait();

  //cocobot_trajectory_set_opponent_detection(0);
  //cocobot_trajectory_goto_xy(x_k * 200, 100, 10000);

  //cocobot_trajectory_wait();
  //cocobot_trajectory_set_opponent_detection(1);


  cocobot_trajectory_goto_d(150, 2500);
  cocobot_trajectory_goto_xy_forward(x_k * (1500 - 400), strat_little_dune_get_y(), 10000);

  cocobot_trajectory_wait();

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_little_dune_register(void)
{
  cocobot_action_scheduler_add_action(
                                    "little_dune",
                                    strat_little_dune_get_score(),
                                    strat_little_dune_pos,
                                    strat_little_dune_get_exec_time(),
                                    strat_little_dune_get_success_proba(),
                                    strat_little_dune_preexec,
                                    strat_little_dune_exec,
                                    strat_little_dune_cleanup,
                                    NULL,
                                    NULL);
}
