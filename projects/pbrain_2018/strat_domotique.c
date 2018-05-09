#include <cocobot.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "strat_domotique.h"

static unsigned int strat_domotique_get_score()
{
    return 30;
}

static float strat_domotique_get_x()
{
    float target = 375;
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_domotique_get_y() 
{
    return 725;
}

static float strat_domotique_get_a()
{
    return -90;
}

static void strat_domotique_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_domotique_get_x();
    *y = strat_domotique_get_y();
    *a = strat_domotique_get_a();
}

float strat_domotique_get_exec_time()
{
    return 3000;
}


float strat_domotique_get_success_proba()
{
    return 0.9;
}

static cocobot_action_callback_result_t strat_domotique_preexec(void * arg)
{
    (void)arg;
    //TODO: Open the actuator
    cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_domotique_cleanup(void * arg)
{
    (void)arg;
    cocobot_trajectory_goto_d(250, 2500);
    cocobot_trajectory_wait();

    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_domotique_exec(void * arg)
{
    (void)arg;
    cocobot_trajectory_goto_d(-250, 5000);
    cocobot_trajectory_wait();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_domotique_register(void)
{
    cocobot_action_scheduler_add_action(
            "domotique_on",
            strat_domotique_get_score(),
            strat_domotique_pos,
            strat_domotique_get_exec_time(),
            strat_domotique_get_success_proba(),
            strat_domotique_preexec,
            strat_domotique_exec,
            strat_domotique_cleanup,
            NULL,
            NULL);
}
