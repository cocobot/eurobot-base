#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include "strat_fish.h"
#include "meca_fish.h"
#include "meca_seashell.h"

static int last_depose_idx;
static int last_take_idx;
static int fish_taken;
static int seeshell_removed;
static int remaining_fish;

static float strat_fish_take_get_x(void)
{
  float target = 1500 - 610 - last_take_idx * 100;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_fish_take_get_y(void)
{
  return -(1000 - 140);
}

static float strat_fish_take_get_a(void)
{
  return -90;
}

static void strat_fish_take_pos(void *arg, float *x, float *y, float *a)
{
  (void)arg;
  *x = strat_fish_take_get_x();
  *y = strat_fish_take_get_y();
  *a = strat_fish_take_get_a();
}

static float strat_fish_take_get_exec_time(void)
{
  return 2000;
}

float strat_fish_take_get_success_proba(void)
{
  return 0.75;
}

float strat_fish_take_get_score(void)
{
  return 5;
}

static cocobot_action_callback_result_t strat_fish_take_preexec(void * arg)
{
  (void)arg;


  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_take_cleanup(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static int strat_fish_take_unlocked(void)
{
  return !fish_taken;
}


static cocobot_action_callback_result_t strat_fish_take_exec(void * arg)
{
  (void)arg;

  int x_k = 1;
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    x_k = -1;
  }


  void * ptr = cocobot_game_state_get_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION);
  int conf = *((unsigned int *)ptr);
  
  if((conf == 0) || (conf == 5))
  {
    if(!seeshell_removed)
    {
      //remove shell
      meca_seashell_open();
      cocobot_trajectory_goto_xy(x_k * 800, -850, 5000);
      cocobot_trajectory_goto_a(90, 2000);
      cocobot_trajectory_goto_d(225, 3000);
      cocobot_trajectory_goto_d(-150, 3000);
      cocobot_trajectory_wait();
      meca_seashell_close();


      cocobot_trajectory_goto_xy_backward(strat_fish_take_get_x(), strat_fish_take_get_y(), 5000);
      cocobot_trajectory_goto_a(strat_fish_take_get_a(), 5000);
      cocobot_trajectory_wait();

      seeshell_removed = 1;
    }
  }


  cocobot_trajectory_goto_d(140, 1500);
  cocobot_trajectory_wait();
  meca_fish_walk(1);

  int i;
  for(i = 0; i < 3; i += 1) 
  {
    switch(cocobot_game_state_get_color())
    {
      case COCOBOT_GAME_STATE_COLOR_POS:
        if((i % 2) == 1)
        {
          meca_fish_sweep_right(1, i == 0);
        }
        else
        {
          meca_fish_sweep_left(1, i == 0);
        }
        break;

      case COCOBOT_GAME_STATE_COLOR_NEG:
        if((i % 2) == 0)
        {
          meca_fish_sweep_right(1, i == 0);
        }
        else
        {
          meca_fish_sweep_left(1, i == 0);
        }
        break;
    }

    vTaskDelay(250 / portTICK_PERIOD_MS);

    if(meca_fish_is_catch())
    {
      fish_taken = 1;
      break;
    }
  }

  meca_fish_walk(0);
  cocobot_trajectory_goto_d(-200, 5000);
  cocobot_trajectory_wait();

  last_take_idx += 1;
  last_take_idx %= 3;

  if(fish_taken)
  {
    remaining_fish -= 1;
    if(remaining_fish == 0)
    {
      return COCOBOT_RETURN_ACTION_SUCCESS;
    }
    else
    {
      return COCOBOT_RETURN_ACTION_SUCCESS_BUT_DO_IT_AGAIN;
    }
  }

  return COCOBOT_RETURN_ACTION_UNKNOWN_FAILURE;        
}

static float strat_fish_release_get_x(void)
{
  float target = 400 - 50 * last_depose_idx;

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    return -target;
  }
  return target;
}

static float strat_fish_release_get_y(void)
{
  return -800;
}

static float strat_fish_release_get_a(void)
{
  return -90;
}

static void strat_fish_release_pos(void *arg, float *x, float *y, float *a)
{
  (void)arg;
  *x = strat_fish_release_get_x();
  *y = strat_fish_release_get_y();
  *a = strat_fish_release_get_a();
}

static float strat_fish_release_get_exec_time(void)
{
  return 2000;
}

float strat_fish_release_get_success_proba(void)
{
  return 0.90;
}

float strat_fish_release_get_score(void)
{
  return 5;
}

static cocobot_action_callback_result_t strat_fish_release_preexec(void * arg)
{
  (void)arg;


  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_release_cleanup(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static int strat_fish_release_unlocked(void)
{
  return fish_taken;
}

static cocobot_action_callback_result_t strat_fish_release_exec(void * arg)
{
  (void)arg;

  cocobot_trajectory_goto_d(50, 2500);
  cocobot_trajectory_goto_d(100, 750);

  cocobot_trajectory_wait();

  meca_fish_prepare(0);

  cocobot_trajectory_goto_d(-250, 5000);
  cocobot_trajectory_wait();

  meca_fish_close(0);

  fish_taken = 0;
  return COCOBOT_RETURN_ACTION_SUCCESS_BUT_DO_IT_AGAIN;
}

void strat_fish_register(void)
{
  last_depose_idx = 0;
  last_take_idx = 0;
  fish_taken = 0;
  seeshell_removed = 0;
  remaining_fish = 4;

  cocobot_action_scheduler_add_action(
                                    "take_fish",
                                    strat_fish_take_get_score(),
                                    strat_fish_take_pos,
                                    strat_fish_take_get_exec_time(),
                                    strat_fish_take_get_success_proba(),
                                    strat_fish_take_preexec,
                                    strat_fish_take_exec,
                                    strat_fish_take_cleanup,
                                    NULL,
                                    strat_fish_take_unlocked);

  cocobot_action_scheduler_add_action(
                                    "release_fish",
                                    strat_fish_release_get_score(),
                                    strat_fish_release_pos,
                                    strat_fish_release_get_exec_time(),
                                    strat_fish_release_get_success_proba(),
                                    strat_fish_release_preexec,
                                    strat_fish_release_exec,
                                    strat_fish_release_cleanup,
                                    NULL,
                                    strat_fish_release_unlocked);
}
