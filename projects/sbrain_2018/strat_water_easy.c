#include <cocobot.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "strat_water_easy.h"
#include "meca_water.h"

static uint8_t isFlooded = 0;

static unsigned int strat_water_easy_take_get_score()
{
    return 10;
}

static float strat_water_easy_take_get_x()
{
    float target = 1225;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_easy_take_get_y()
{
    return 150;
}

static float strat_water_easy_take_get_a()
{
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
        return 0;
    else
        return 180;
}

static void strat_water_easy_take_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_water_easy_take_get_x();
    *y = strat_water_easy_take_get_y();
    *a = strat_water_easy_take_get_a();
}

float strat_water_easy_take_get_exec_time()
{
    return 3000;
}


float strat_water_easy_take_get_success_proba()
{
    return 1;
}

static cocobot_action_callback_result_t strat_water_easy_take_preexec(void * arg)
{
    (void)arg;
    //TODO: Set loader in ready position?
    cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_easy_take_cleanup(void * arg)
{
    (void)arg;
    //Get away from the water castle
    cocobot_trajectory_goto_d(250, 2500);
    cocobot_trajectory_wait();
    cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_easy_take_exec(void * arg)
{
    (void)arg;
    //Move just under the water castle
    cocobot_trajectory_goto_d(-200, 2500);
    cocobot_trajectory_wait();

    //TBD:Get the water
    
    //The robot is now under water
    isFlooded = 1;

    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static unsigned int strat_water_easy_shoot_get_score()
{
    return 40;
}

static float strat_water_easy_shoot_get_x()
{
    float target = 1100;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_easy_shoot_get_y()
{
    return 500;
}

static float strat_water_easy_shoot_get_a()
{
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
        return 120;
    else
        return 60;
}

static void strat_water_easy_shoot_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_water_easy_shoot_get_x();
    *y = strat_water_easy_shoot_get_y();
    *a = strat_water_easy_shoot_get_a();
}

float strat_water_easy_shoot_get_exec_time()
{
    return 4000;
}

float strat_water_easy_shoot_get_success_proba()
{
    return 0.85;
}

static cocobot_action_callback_result_t strat_water_easy_shoot_preexec(void * arg)
{
    (void)arg;
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_easy_shoot_cleanup(void * arg)
{
    (void)arg;
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_easy_shoot_exec(void * arg)
{
    (void)arg;
    //TODO: Open fire!
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static int strat_water_easy_shoot_unlocked(void)
{
    return isFlooded;
}

void strat_water_easy_register(void)
{
    cocobot_action_scheduler_add_action(
            "get_water_easy",
            strat_water_easy_take_get_score(),
            strat_water_easy_take_pos,
            strat_water_easy_take_get_exec_time(),
            strat_water_easy_take_get_success_proba(),
            strat_water_easy_take_preexec,
            strat_water_easy_take_exec,
            strat_water_easy_take_cleanup,
            NULL,
            NULL);

    cocobot_action_scheduler_add_action(
            "shoot_water_easy",
            strat_water_easy_shoot_get_score(),
            strat_water_easy_shoot_pos,
            strat_water_easy_shoot_get_exec_time(),
            strat_water_easy_shoot_get_success_proba(),
            strat_water_easy_shoot_preexec,
            strat_water_easy_shoot_exec,
            strat_water_easy_shoot_cleanup,
            NULL,
            strat_water_easy_shoot_unlocked);
}
