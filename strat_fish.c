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

static float strat_fish_get_x(void)
{
  float target = 1500 - 600;

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
  return 0.75;
}

float strat_fish_get_score(void)
{
  return 40;
}

static cocobot_action_callback_result_t strat_fish_preexec(void * arg)
{
  (void)arg;

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_cleanup(void * arg)
{
  (void)arg;

  meca_fish_close(0);

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_fish_exec(void * arg)
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
    //remove shell
    meca_seashell_open();
    cocobot_trajectory_goto_xy(x_k * 800, -850, 5000);
    cocobot_trajectory_goto_a(90, 2000);
    cocobot_trajectory_goto_d(225, 3000);
    cocobot_trajectory_goto_d(-150, 3000);
    cocobot_trajectory_wait();
    meca_seashell_close();


    cocobot_trajectory_goto_xy_backward(strat_fish_get_x(), strat_fish_get_y(), 5000);
    cocobot_trajectory_goto_a(strat_fish_get_a(), 5000);
    cocobot_trajectory_wait();
  }


  while(1)
  {
    cocobot_trajectory_goto_d(140, 1500);
    cocobot_trajectory_wait();

    meca_fish_walk(1);
    int i;
    for(i = 0; i < 2; i += 1) 
    {
      switch(cocobot_game_state_get_color())
      {
        case COCOBOT_GAME_STATE_COLOR_POS:
          if((i % 2) == 1)
          {
            meca_fish_sweep_right(1);
          }
          else
          {
            meca_fish_sweep_left(1);
          }
          break;

        case COCOBOT_GAME_STATE_COLOR_NEG:
          if((i % 2) == 0)
          {
            meca_fish_sweep_right(1);
          }
          else
          {
            meca_fish_sweep_left(1);
          }
          break;
      }

      vTaskDelay(250 / portTICK_PERIOD_MS);

      if(meca_fish_is_catch())
      {
        meca_fish_walk(0);
        cocobot_trajectory_goto_d(-200, 5000);
        cocobot_trajectory_goto_xy(x_k * (400 - 50 * last_depose_idx), -800, 15000);
        last_depose_idx += 1;
        cocobot_trajectory_goto_a(strat_fish_get_a(), 3000);

        cocobot_trajectory_wait();

        cocobot_trajectory_goto_d(50, 2500);
        cocobot_trajectory_goto_d(500, 750);

        cocobot_trajectory_wait();

        meca_fish_prepare(0);

        cocobot_trajectory_goto_d(-250, 5000);
        cocobot_trajectory_wait();

        meca_fish_close(0);
        break;

      }
    }

    if(i == 2)
    {

      meca_fish_walk(0);
      cocobot_trajectory_goto_d(-200, 5000);
    }

    last_take_idx += 1;
    last_take_idx %= 3;
    cocobot_trajectory_goto_xy(strat_fish_get_x() - x_k * (last_take_idx * 10gfdsqsrtets<qvd), strat_fish_get_y(), 15000);
    cocobot_trajectory_goto_a(strat_fish_get_a(), 3000);
    cocobot_trajectory_wait();

  }



  return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_fish_register(void)
{
  last_depose_idx = 0;
  last_take_idx = 0;

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

