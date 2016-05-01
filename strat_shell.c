#include <cocobot.h>
#include <stdio.h>
#include <math.h>
#include "strat_shell.h"
#include "meca_seashell.h"

#define SHELL_NUMBER 22
#define SHELL_ACTION_NAME_LENGTH 8
#define SHELL_TARGET_AREA_X 1300
#define SHELL_TARGET_AREA_Y -50

#define SHELL_BASE_PROBA_COLOR 1
#define SHELL_BASE_PROBA_WHITE 0.9

char action_names[SHELL_NUMBER][SHELL_ACTION_NAME_LENGTH];

static unsigned int strat_shell_get_score(int shell)
{
  (void)shell; //each shell is equal
  return 2;
}

static float strat_shell_get_x(int shell)
{
  float x = 10000;
  switch(shell)
  {
    case 0:
    case 6:
      x = -1300;
      break;

    case 1:
    case 7:
    case 15:
      x = -800;
      break;

    case 2:
    case 9:
    case 17:
      x = 800;
      break;

    case 3:
    case 10:
      x = 1300;
      break;

    case 4:
      x = -600;
      break;

    case 5:
      x = 600;
      break;

    case 8:
    case 16:
      x = 0;
      break;

    case 11:
      x = -300;
      break;

    case 12:
      x = 300;
      break;

    case 13:
    case 18:
      x = -1430;
      break;

    case 14:
    case 21:
      x = 1430;
      break;

    case 19:
      x = -1310;
      break;

    case 20:
      x = 1310;
      break;
  }

  return x;
}

static float strat_shell_get_y(int shell)
{
  float y = 10000;
  switch(shell)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      y = -250;
      break;

    case 4:
    case 5:
      y = -450;
      break;

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      y = -550;
      break;

    case 11:
    case 12:
      y = -650;
      break;

    case 13:
    case 14:
      y = -810;
      break;

    case 15:
    case 16:
    case 17:
      y = -850;
      break;

    case 18:
    case 19:
    case 21:
      y = -930;
      break;
  }

  return y;
}

static float strat_shell_get_a(int shell)
{
  (void)shell; //each shell is equal

  return NAN;
}

float strat_shell_get_exec_time(int shell)
{
  float target_x = 0;
  float target_y = 0;
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_POS:
      target_x = SHELL_TARGET_AREA_X;
      target_y = SHELL_TARGET_AREA_Y;
      break;

    case COCOBOT_GAME_STATE_COLOR_NEG:
      target_x = -SHELL_TARGET_AREA_X;
      target_y = SHELL_TARGET_AREA_Y;
      break;
  }

  float shell_x = strat_shell_get_x(shell);
  float shell_y = strat_shell_get_y(shell);

  float dx = shell_x - target_x;
  float dy = shell_y - target_y;

  float approximate_linear_distance = sqrt(dx * dx + dy * dy);
  
  return approximate_linear_distance / cocobot_asserv_get_linear_speed() + 2500; //+2500 beacause we need to go back at the end.
}

float strat_shell_compute_base_proba(int conf, int shell, cocobot_game_state_color_t color)
{
  switch(conf)
  {
    case 1:
      switch(shell)
      {
        case 0:
        case 3:
        case 6:
        case 8:
        case 10:
        case 16:
          return SHELL_BASE_PROBA_WHITE;

        case 4:
        case 12:
        case 13:
        case 18:
        case 19:
          if(color == COCOBOT_GAME_STATE_COLOR_NEG)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }

        case 5:
        case 11:
        case 14:
        case 20:
        case 21:
          if(color == COCOBOT_GAME_STATE_COLOR_POS)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }
      }
      break;

    case 2:
      switch(shell)
      {
        case 6:
        case 8:
        case 10:
        case 16:
        case 18:
        case 21:
          return SHELL_BASE_PROBA_WHITE;

        case 0:
        case 4:
        case 11:
        case 13:
        case 19:
          if(color == COCOBOT_GAME_STATE_COLOR_NEG)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }

        case 3:
        case 5:
        case 12:
        case 14:
        case 20:
          if(color == COCOBOT_GAME_STATE_COLOR_POS)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }
      }
      break;

    case 3:
      switch(shell)
      {
        case 6:
        case 7:
        case 9:
        case 10:
        case 18:
        case 21:
          return SHELL_BASE_PROBA_WHITE;

        case 0:
        case 1:
        case 11:
        case 13:
        case 19:
          if(color == COCOBOT_GAME_STATE_COLOR_NEG)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }

        case 2:
        case 3:
        case 12:
        case 14:
        case 20:
          if(color == COCOBOT_GAME_STATE_COLOR_POS)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }
      }
      break;

    case 4:
      switch(shell)
      {
        case 7:
        case 9:
        case 11:
        case 12:
        case 18:
        case 21:
          return SHELL_BASE_PROBA_WHITE;

        case 0:
        case 1:
        case 6:
        case 14:
        case 20:
          if(color == COCOBOT_GAME_STATE_COLOR_NEG)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }

        case 2:
        case 3:
        case 10:
        case 13:
        case 19:
          if(color == COCOBOT_GAME_STATE_COLOR_POS)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }
      }
      break;

    case 5:
      switch(shell)
      {
        case 15:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
          return SHELL_BASE_PROBA_WHITE;

        case 0:
        case 1:
        case 6:
        case 9:
        case 13:
          if(color == COCOBOT_GAME_STATE_COLOR_NEG)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }

        case 2:
        case 3:
        case 7:
        case 10:
        case 14:
          if(color == COCOBOT_GAME_STATE_COLOR_POS)
          {
            return SHELL_BASE_PROBA_COLOR;
          }
          else
          {
            return 0;
          }
      }
      break;
  }

  return 0;
}

float strat_shell_get_success_proba(int shell)
{
  void * ptr = cocobot_game_state_get_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION);
  int conf = *((unsigned int *)ptr);
  
  float success = 0;
  
  if(conf == 0)
  {
    int i;
    for(i = 1; i < 6; i += 1)
    {
      success += strat_shell_compute_base_proba(i, shell, cocobot_game_state_get_color());
    }
    success /= 5;
  }
  else
  {
    success = strat_shell_compute_base_proba(conf, shell, cocobot_game_state_get_color());
  }

  switch(shell)
  {
    case 13:
    case 14:
    case 18:
    case 19:
    case 20:
    case 21:
      success = 0;
      break;
  }

  float x = strat_shell_get_x(shell);

  if(x == 0)
  {
    success *= 0.8;
  }
  else
  {
    switch(cocobot_game_state_get_color())
    {
      case COCOBOT_GAME_STATE_COLOR_NEG:
        if(x > 0)
        {
          success *= 0.5;
        }
        break;

      case COCOBOT_GAME_STATE_COLOR_POS:
        if(x < 0)
        {
          success *= 0.5;
        }
        break;
    }
  }

  return success;
}

static cocobot_action_callback_result_t strat_shell_preexec(void * arg)
{
  (void)arg;

  meca_seashell_open();

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_shell_cleanup(void * arg)
{
  (void)arg;

  meca_seashell_close();

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_shell_exec(void * arg)
{
  (void)arg;

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy(SHELL_TARGET_AREA_X, SHELL_TARGET_AREA_Y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
      break;

    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy(-SHELL_TARGET_AREA_X, SHELL_TARGET_AREA_Y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
      break;
  }

  cocobot_trajectory_wait();

  meca_seashell_close();

  cocobot_trajectory_goto_d(-200, 5000);
  cocobot_trajectory_result_t res = cocobot_trajectory_wait();

  if(res == COCOBOT_TRAJECTORY_STOPPED_BEFORE_END)
  {
    return COCOBOT_RETURN_ACTION_SUCCESS_BUT_IM_LOST;
  }

  return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_shell_register(void)
{
  int i;
  for(i = 0; i < SHELL_NUMBER; i += 1)
  {
    unsigned int score = strat_shell_get_score(i);
    float success_proba = strat_shell_get_success_proba(i);
    if((score > 0) && (success_proba > 0))
    {
      char * action_name = action_names[i];
      snprintf(action_name,SHELL_ACTION_NAME_LENGTH, "shell_%d", i);
      cocobot_action_scheduler_add_action(
                                        action_name,
                                        score,
                                        strat_shell_get_x(i),
                                        strat_shell_get_y(i),
                                        strat_shell_get_a(i),
                                        strat_shell_get_exec_time(i),
                                        success_proba,
                                        strat_shell_preexec,
                                        strat_shell_exec,
                                        strat_shell_cleanup,
                                        NULL,
                                        NULL);
    }
  }
}
