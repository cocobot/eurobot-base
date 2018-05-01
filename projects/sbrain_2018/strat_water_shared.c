#include <cocobot.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "strat_water_shared.h"
#include "meca_water.h"

static uint8_t isFlooded = 0;

static unsigned int strat_water_shared_take_get_score()
{
    return 10;
}

static float strat_water_shared_take_get_x()
{
    float target = -900;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_shared_take_get_y()
{
    return -725;
}

static float strat_water_shared_take_get_a()
{
    return 90;
}

static void strat_water_shared_take_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_water_shared_take_get_x();
    *y = strat_water_shared_take_get_y();
    *a = strat_water_shared_take_get_a();
}

float strat_water_shared_take_get_exec_time()
{
    return 3000;
}


float strat_water_shared_take_get_success_proba()
{
    return 0.75;
}

static cocobot_action_callback_result_t strat_water_shared_take_preexec(void * arg)
{
    (void)arg;
    //TODO: Set loader in ready position?
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_take_cleanup(void * arg)
{
    (void)arg;
    //Get away from the water castle
    cocobot_trajectory_goto_d(250, 2500);
    cocobot_trajectory_wait();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_take_exec(void * arg)
{
    (void)arg;
    //Move just under the water castle
    cocobot_trajectory_goto_d(-200, 2500);
    cocobot_trajectory_wait();

    //TBD:Get the water
    
    //The robot is now full of water
    isFlooded = 1;

    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static unsigned int strat_water_shared_shoot_get_score()
{
    return 20;
}

static float strat_water_shared_shoot_get_x()
{
    float target = 1100;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_shared_shoot_get_y()
{
    return 500;
}

static float strat_water_shared_shoot_get_a()
{
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
        return 120;
    else
        return 60;
}

static void strat_water_shared_shoot_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_water_shared_shoot_get_x();
    *y = strat_water_shared_shoot_get_y();
    *a = strat_water_shared_shoot_get_a();
}

float strat_water_shared_shoot_get_exec_time()
{
    return 4000;
}



float strat_water_shared_shoot_get_success_proba()
{
    return 0.50;
}

static cocobot_action_callback_result_t strat_water_shared_shoot_preexec(void * arg)
{
    (void)arg;
    //Get away from the water castle
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_shoot_cleanup(void * arg)
{
    (void)arg;
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_shoot_exec(void * arg)
{
    (void)arg;
    //TODO: Open fire!
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static int strat_water_shared_shoot_unlocked(void)
{
    return isFlooded;
}

static unsigned int strat_water_shared_recycle_get_score()
{
    return 20;
}

static float strat_water_shared_recycle_get_x()
{
    float target = 175;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_shared_recycle_get_y()
{
    return -575;
}

static float strat_water_shared_recycle_get_a()
{
    return 90;
}

static void strat_water_shared_recycle_pos(void *arg, float *x, float *y, float *a)
{
    (void)arg;
    *x = strat_water_shared_recycle_get_x();
    *y = strat_water_shared_recycle_get_y();
    *a = strat_water_shared_recycle_get_a();
}

float strat_water_shared_recycle_get_exec_time()
{
    return 2000;
}


float strat_water_shared_recycle_get_success_proba()
{
    return 0.50;
}

static cocobot_action_callback_result_t strat_water_shared_recycle_preexec(void * arg)
{
    (void)arg;
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_recycle_cleanup(void * arg)
{
    (void)arg;
    cocobot_trajectory_goto_d(250, 2500);
    cocobot_trajectory_wait();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_shared_recycle_exec(void * arg)
{
    (void)arg;
    cocobot_trajectory_goto_d(-200, 2500);
    cocobot_trajectory_wait();
    //TODO: Dump wasted water
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static int strat_water_shared_recycle_unlocked(void)
{
    return isFlooded;
}

void strat_water_shared_register(void)
{
    cocobot_action_scheduler_add_action(
            "get_water_shared",
            strat_water_shared_take_get_score(),
            strat_water_shared_take_pos,
            strat_water_shared_take_get_exec_time(),
            strat_water_shared_take_get_success_proba(),
            strat_water_shared_take_preexec,
            strat_water_shared_take_exec,
            strat_water_shared_take_cleanup,
            NULL,
            NULL);

    cocobot_action_scheduler_add_action(
            "shoot_water_shared",
            strat_water_shared_shoot_get_score(),
            strat_water_shared_shoot_pos,
            strat_water_shared_shoot_get_exec_time(),
            strat_water_shared_shoot_get_success_proba(),
            strat_water_shared_shoot_preexec,
            strat_water_shared_shoot_exec,
            strat_water_shared_shoot_cleanup,
            NULL,
            strat_water_shared_shoot_unlocked);

    cocobot_action_scheduler_add_action(
            "recycle_water_shared",
            strat_water_shared_recycle_get_score(),
            strat_water_shared_recycle_pos,
            strat_water_shared_recycle_get_exec_time(),
            strat_water_shared_recycle_get_success_proba(),
            strat_water_shared_recycle_preexec,
            strat_water_shared_recycle_exec,
            strat_water_shared_recycle_cleanup,
            NULL,
            strat_water_shared_recycle_unlocked);
}
