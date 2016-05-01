#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include "strat_sand.h"

static float strat_sand_get_x(void)
{
  float target = 850 + 300;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_sand_get_y(void)
{
  return 100;
}

static float strat_sand_get_a(void)
{
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return 180;
  }
  return 0;
}

static float strat_sand_get_exec_time(void)
{
  return 5000;
}

float strat_sand_get_success_proba(void)
{
  return 1;
}

float strat_sand_get_score(void)
{
  return 16;
}

static cocobot_action_callback_result_t strat_sand_preexec(void * arg)
{
  (void)arg;

  cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);

  return 0;
}

static cocobot_action_callback_result_t strat_sand_cleanup(void * arg)
{
  (void)arg;

  cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);
  
  return 0;
}

static cocobot_action_callback_result_t strat_sand_exec(void * arg)
{
  (void)arg;

  int x_k = 1;
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    x_k = -1;
  }

  cocobot_trajectory_goto_xy(x_k * 700, 100, -1);
  cocobot_trajectory_goto_xy(x_k * 400, -100, -1);
  cocobot_trajectory_goto_xy(x_k * 200, -100, -1);
  cocobot_trajectory_goto_d(500, -1);

  cocobot_trajectory_wait();

  return 1;
}

void strat_sand_register(void)
{
  cocobot_action_scheduler_add_action(
                                    "sand",
                                    strat_sand_get_score(),
                                    strat_sand_get_x(),
                                    strat_sand_get_y(),
                                    strat_sand_get_a(),
                                    strat_sand_get_exec_time(),
                                    strat_sand_get_success_proba(),
                                    strat_sand_preexec,
                                    strat_sand_exec,
                                    strat_sand_cleanup,
                                    NULL,
                                    NULL);
}
