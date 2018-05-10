#include <cocobot.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "strat_water.h"
#include "meca_water.h"

#define NBR_OF_WATER 2

typedef enum 
{
    WATER_EASY = 0,
    WATER_LESS_EASY,
}waterDistributor_e;

static uint8_t isFlooded[NBR_OF_WATER] = {0};

static unsigned int strat_water_take_get_score(waterDistributor_e water)
{
    if(water == WATER_EASY)
        return 50;
    else
        return 70;
}

static float strat_water_take_get_x(waterDistributor_e water)
{
    float target = 0;
    if(water == WATER_EASY)
        target = 1175;
    else
        target = -900;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_water_take_get_y(waterDistributor_e water)
{
    float target = 0;
    if(water == WATER_EASY)
        target = 160;
    else
        target = -675;//TODO

    return target;
}

static float strat_water_take_get_a(waterDistributor_e water)
{
    if(water == WATER_EASY)
    {
        if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
            return 0;
        else
            return 180;
    }
    else
        return 90;
}

static void strat_water_take_pos(void *arg, float *x, float *y, float *a)
{
    waterDistributor_e water = (waterDistributor_e)arg;
    *x = strat_water_take_get_x(water);
    *y = strat_water_take_get_y(water);
    *a = strat_water_take_get_a(water);
}

float strat_water_take_get_exec_time(waterDistributor_e water)
{
    (void) water;
    return 3000;
}


float strat_water_take_get_success_proba(waterDistributor_e water)
{
    if(water == WATER_EASY)
        return 0.65;
    else
        return 0.55;
}

static cocobot_action_callback_result_t strat_water_take_preexec(void * arg)
{
    (void)arg;
    //TODO: Set loader in ready position?
    cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_take_cleanup(void * arg)
{
    waterDistributor_e water = (waterDistributor_e) arg;
    cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);

    if(water == WATER_LESS_EASY)
    {
        if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
            cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_5);
        else
            cocobot_pathfinder_conf_remove_game_element(CUBE_CROSS_4);
    }

    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_water_take_exec(void * arg)
{
    waterDistributor_e water = (waterDistributor_e )arg;
    //Move just under the water castle
    cocobot_trajectory_goto_d(-130, 2500);
    cocobot_trajectory_wait();

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_a(180, 2500);
    }
    else
    {
        cocobot_trajectory_goto_a(0, 2500);
    }
    cocobot_trajectory_wait();
    meca_water_take_from_distributor();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    //Get away from the water castle
    cocobot_trajectory_goto_d(250, 2500);
    cocobot_trajectory_wait();

    //The robot is now under water
    isFlooded[water] = 1;

    cocobot_game_state_add_points_to_score(10);
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

//static unsigned int strat_water_shoot_get_score(waterDistributor_e water)
//{
//    if(water == WATER_EASY)
//        return 40;
//    else
//        return 20;
//}
//
//static float strat_water_shoot_get_x(waterDistributor_e water)
//{
//    (void)water;
//    float target = 1100;
//
//    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
//    {
//        return -target;
//    }
//    return target;
//}
//
//static float strat_water_shoot_get_y(waterDistributor_e water)
//{
//    (void)water;
//    return 500;
//}
//
//static float strat_water_shoot_get_a(waterDistributor_e water)
//{
//    (void)water;
//    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
//        return 120;
//    else
//        return 60;
//}
//
//static void strat_water_shoot_pos(void *arg, float *x, float *y, float *a)
//{
//    waterDistributor_e water = (waterDistributor_e )arg;
//    *x = strat_water_shoot_get_x(water);
//    *y = strat_water_shoot_get_y(water);
//    *a = strat_water_shoot_get_a(water);
//}
//
//float strat_water_shoot_get_exec_time(waterDistributor_e water)
//{
//    (void)water;
//    return 4000;
//}
//
//float strat_water_shoot_get_success_proba(waterDistributor_e water)
//{
//    if(water == WATER_EASY)
//        return 0.85;
//    else
//        return 0.5;
//}
//
//static cocobot_action_callback_result_t strat_water_shoot_preexec(void * arg)
//{
//    (void)arg;
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static cocobot_action_callback_result_t strat_water_shoot_cleanup(void * arg)
//{
//    (void)arg;
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static cocobot_action_callback_result_t strat_water_shoot_exec(void * arg)
//{
//    //TODO: Open fire!
//    waterDistributor_e water = (waterDistributor_e )arg;
//    if(water == WATER_EASY)
//    {
//        //shoot for 8 balls
//        meca_water_shoot_all();
//        cocobot_game_state_add_points_to_score(40);
//    }
//    else
//    {
//        //shoot for 4 balls.
//        meca_water_shoot_left();
//        cocobot_game_state_add_points_to_score(20);
//    }
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static int strat_water_shoot_unlocked(void * arg)
//{
//    waterDistributor_e water = (waterDistributor_e) arg;
//    return isFlooded[water];
//}

//static unsigned int strat_water_recycle_get_score()
//{
//    return 40;
//}
//
//static float strat_water_recycle_get_x()
//{
//    float target = -475;
//
//    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
//    {
//        return -target;
//    }
//    return target;
//}
//
//static float strat_water_recycle_get_y()
//{
//    return -575;
//}
//
//static float strat_water_recycle_get_a()
//{
//    return 90;
//}
//
//static void strat_water_recycle_pos(void *arg, float *x, float *y, float *a)
//{
//    (void)arg;
//    *x = strat_water_recycle_get_x();
//    *y = strat_water_recycle_get_y();
//    *a = strat_water_recycle_get_a();
//}
//
//float strat_water_recycle_get_exec_time()
//{
//    return 2000;
//}
//
//
//float strat_water_recycle_get_success_proba()
//{
//    return 0.80;
//}
//
//static cocobot_action_callback_result_t strat_water_recycle_preexec(void * arg)
//{
//    (void)arg;
//    
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static cocobot_action_callback_result_t strat_water_recycle_cleanup(void * arg)
//{
//    (void)arg;
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static cocobot_action_callback_result_t strat_water_recycle_exec(void * arg)
//{
//    (void)arg;
//    cocobot_trajectory_goto_d(-100, 2500);
//    cocobot_trajectory_wait();
//    meca_water_release_bad_water();
//    cocobot_trajectory_goto_d(250, 2500);
//    cocobot_trajectory_wait();
//    cocobot_game_state_add_points_to_score(40);
//    return COCOBOT_RETURN_ACTION_SUCCESS;
//}
//
//static int strat_water_recycle_unlocked(void * arg)
//{
//    (void)arg;
//    return isFlooded[WATER_LESS_EASY];
//}

void strat_water_register(void)
{
    cocobot_action_scheduler_add_action(
            "get_water_easy",
            strat_water_take_get_score(WATER_EASY),
            strat_water_take_pos,
            strat_water_take_get_exec_time(WATER_EASY),
            strat_water_take_get_success_proba(WATER_EASY),
            strat_water_take_preexec,
            strat_water_take_exec,
            strat_water_take_cleanup,
            (void*) WATER_EASY,
            NULL);

    //cocobot_action_scheduler_add_action(
    //        "shoot_water_easy",
    //        strat_water_shoot_get_score(WATER_EASY),
    //        strat_water_shoot_pos,
    //        strat_water_shoot_get_exec_time(WATER_EASY),
    //        strat_water_shoot_get_success_proba(WATER_EASY),
    //        strat_water_shoot_preexec,
    //        strat_water_shoot_exec,
    //        strat_water_shoot_cleanup,
    //        (void*)WATER_EASY,
    //        strat_water_shoot_unlocked);

    cocobot_action_scheduler_add_action(
            "get_water_shared",
            strat_water_take_get_score(WATER_LESS_EASY),
            strat_water_take_pos,
            strat_water_take_get_exec_time(WATER_LESS_EASY),
            strat_water_take_get_success_proba(WATER_LESS_EASY),
            strat_water_take_preexec,
            strat_water_take_exec,
            strat_water_take_cleanup,
            (void*) WATER_LESS_EASY,
            NULL);

    //cocobot_action_scheduler_add_action(
    //        "shoot_water_shared",
    //        strat_water_shoot_get_score(WATER_LESS_EASY),
    //        strat_water_shoot_pos,
    //        strat_water_shoot_get_exec_time(WATER_LESS_EASY),
    //        strat_water_shoot_get_success_proba(WATER_LESS_EASY),
    //        strat_water_shoot_preexec,
    //        strat_water_shoot_exec,
    //        strat_water_shoot_cleanup,
    //        (void*)WATER_LESS_EASY,
    //        strat_water_shoot_unlocked);

    //cocobot_action_scheduler_add_action(
    //        "recycle_water_shared",
    //        strat_water_recycle_get_score(),
    //        strat_water_recycle_pos,
    //        strat_water_recycle_get_exec_time(),
    //        strat_water_recycle_get_success_proba(),
    //        strat_water_recycle_preexec,
    //        strat_water_recycle_exec,
    //        strat_water_recycle_cleanup,
    //        NULL,
    //        strat_water_recycle_unlocked);
}
