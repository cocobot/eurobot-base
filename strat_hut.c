#include <cocobot.h>
#include "strat_hut.h"

unsigned int strat_hut_get_score(int hut)
{
  (void)hut; //each hut is equal
  return 10;
}

float strat_hut_get_x(int hut)
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

float strat_hut_get_y(int hut)
{
  (void)hut; //each hut is equal
  return 800;
}

float strat_hut_get_a(int hut)
{
  (void)hut; //each hut is equal
  return -90; //we want to hit the door with our back
}

float strat_hut_get_exec_time(int hut)
{
  (void)hut; //each hut is equal
  return 3000; //in ms
}

float strat_hut_get_success_proba(int hut)
{
  return 0.01; //very low. Main robot should have already done the action before us !
}

int strat_hut_action(int hut)
{
  (void)hut; //each hut is equal

  cocobot_trajectory_goto_d(-300, 2);
  cocobot_trajectory_goto_d(200, -1);
  cocobot_trajectory_wait();

  return 1;
}
