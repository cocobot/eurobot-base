#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include <FreeRTOS.h>
#include <task.h>
#include "strat_easy_dune.h"
#include "meca_crimp.h"

static float strat_easy_dune_get_x(void)
{
  float target = 850 + 300;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_easy_dune_get_y(void)
{
  return 20;
}

static float strat_easy_dune_get_a(void)
{
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return 180;
  }
  return 0;
}

static float strat_easy_dune_get_exec_time(void)
{
  return 5000;
}

float strat_easy_dune_get_success_proba(void)
{
  return 0.99;
}

float strat_easy_dune_get_score(void)
{
  return 16;
}

static cocobot_action_callback_result_t strat_easy_dune_preexec(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_easy_dune_cleanup(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_easy_dune_exec(void * arg)
{
  (void)arg;

  int x_k = 1;
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    x_k = -1;
  }

  cocobot_trajectory_goto_xy_backward(x_k * 500, strat_easy_dune_get_y(), 10000);

  cocobot_trajectory_wait();

  cocobot_trajectory_goto_d(150, 2500);
  //cocobot_trajectory_goto_xy_forward(x_k * (1500 - 400), strat_easy_dune_get_y(), 10000);

  cocobot_trajectory_wait();

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_easy_dune_register(void)
{
  cocobot_action_scheduler_add_action(
                                    "easy_dune",
                                    strat_easy_dune_get_score(),
                                    strat_easy_dune_get_x(),
                                    strat_easy_dune_get_y(),
                                    strat_easy_dune_get_a(),
                                    strat_easy_dune_get_exec_time(),
                                    strat_easy_dune_get_success_proba(),
                                    strat_easy_dune_preexec,
                                    strat_easy_dune_exec,
                                    strat_easy_dune_cleanup,
                                    NULL,
                                    NULL);
}

