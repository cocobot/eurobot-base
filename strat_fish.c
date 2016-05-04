#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include "strat_fish.h"
#include "meca_fish.h"

static float strat_fish_get_x(void)
{
  float target = 1500 - 550;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_fish_get_y(void)
{
  return -(1000 - 140);
}

static float strat_fish_get_a(void)
{
  return -90;
}

static float strat_fish_get_exec_time(void)
{
  return 60000;
}

float strat_fish_get_success_proba(void)
{
  return 0.15;
}

float strat_fish_get_score(void)
{
  return 10;
}

static cocobot_action_callback_result_t strat_fish_preexec(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_cleanup(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_exec(void * arg)
{
  (void)arg;

  cocobot_trajectory_goto_d(140, 1500);
  cocobot_trajectory_wait();

  meca_fish_prepare(1);

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_fish_register(void)
{
  cocobot_action_scheduler_add_action(
                                    "fish",
                                    strat_fish_get_score(),
                                    strat_fish_get_x(),
                                    strat_fish_get_y(),
                                    strat_fish_get_a(),
                                    strat_fish_get_exec_time(),
                                    strat_fish_get_success_proba(),
                                    strat_fish_preexec,
                                    strat_fish_exec,
                                    strat_fish_cleanup,
                                    NULL,
                                    NULL);
}

