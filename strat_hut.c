#include <cocobot.h>
#include "strat_hut.h"

#define STRAT_HUT_VIOLET_LEFT     0
#define STRAT_HUT_VIOLET_RIGHT    1
#define STRAT_HUT_GREEN_LEFT      2
#define STRAT_HUT_GREEN_RIGHT     3


static unsigned int strat_hut_get_score(int hut)
{
  (void)hut; //each hut is equal
  return 10;
}

static float strat_hut_get_x(int hut)
{
  switch(hut)
  {
    case STRAT_HUT_VIOLET_LEFT:
      return -1200;

    case STRAT_HUT_VIOLET_RIGHT:
      return -900;

    case STRAT_HUT_GREEN_LEFT:
      return 900;

    case STRAT_HUT_GREEN_RIGHT:
      return 1200;
  }

  return 10000; //unvalid hut has been requested.
}

static float strat_hut_get_y(int hut)
{
  (void)hut; //each hut is equal
  return 700;
}

static float strat_hut_get_a(int hut)
{
  (void)hut; //each hut is equal
  return -90; //we want to hit the door with our back
}

static float strat_hut_get_exec_time(int hut)
{
  (void)hut; //each hut is equal
  return 3000; //in ms
}

static float strat_hut_get_success_proba(int hut)
{
  return 0.01; //very low. Main robot should have already done the action before us !
}

static int strat_hut_action(void * arg)
{
  (void)arg; //each hut is equal

  cocobot_trajectory_goto_d(-10400, 2000);
  cocobot_trajectory_goto_d(300, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  cocobot_trajectory_wait();

  return 1;
}

void strat_hut_register(void)
{
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
    cocobot_action_scheduler_add_action(
                                        "hut 0",
                                        strat_hut_get_score(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_get_x(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_get_y(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_get_a(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_get_exec_time(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_get_success_proba(STRAT_HUT_VIOLET_LEFT),
                                        strat_hut_action,
                                        (void *)STRAT_HUT_VIOLET_LEFT,
                                        NULL);

    cocobot_action_scheduler_add_action(
                                        "hut 1",
                                        strat_hut_get_score(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_get_x(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_get_y(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_get_a(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_get_exec_time(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_get_success_proba(STRAT_HUT_VIOLET_RIGHT),
                                        strat_hut_action,
                                        (void *)STRAT_HUT_VIOLET_RIGHT,
                                        NULL);
  }

  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_POS)
  {
    cocobot_action_scheduler_add_action(
                                        "hut 2",
                                        strat_hut_get_score(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_get_x(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_get_y(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_get_a(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_get_exec_time(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_get_success_proba(STRAT_HUT_GREEN_LEFT),
                                        strat_hut_action,
                                        (void *)STRAT_HUT_GREEN_LEFT,
                                        NULL);

    cocobot_action_scheduler_add_action(
                                        "hut 3",
                                        strat_hut_get_score(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_get_x(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_get_y(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_get_a(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_get_exec_time(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_get_success_proba(STRAT_HUT_GREEN_RIGHT),
                                        strat_hut_action,
                                        (void *)STRAT_HUT_GREEN_RIGHT,
                                        NULL);
  }
}
