#include <cocobot.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "strat_bee.h"
#include "meca_bee.h"
#include "meca_water.h"

static unsigned int strat_bee_get_score()
{
    return 55;
}

static float strat_bee_get_x()
{
    float target = 1275;

    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        return -target;
    }
    return target;
}

static float strat_bee_get_y()
{
    float target = -775;

    return target;
}

static float strat_bee_get_a( )
{
    return -90;
}

static void strat_bee_pos(void *arg, float *x, float *y, float *a)
{
    (void) arg;
    *x = strat_bee_get_x();
    *y = strat_bee_get_y();
    *a = strat_bee_get_a();
}

float strat_bee_get_exec_time( )
{
    return 1000;
}


float strat_bee_get_success_proba( )
{
    return 1;
}

static cocobot_action_callback_result_t strat_bee_preexec(void * arg)
{
    (void)arg;
    meca_bee_prepare();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_bee_cleanup(void * arg)
{
    (void) arg;
    meca_bee_init();
    meca_water_activate();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    meca_water_init();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

static cocobot_action_callback_result_t strat_bee_exec(void * arg)
{
    (void) arg;
    // Go strait forward until the end of the table is reached. 
#ifdef AUSBEE_SIM
    int try = 5;
#endif
    while(1)
    {
        cocobot_trajectory_goto_d(20, 1000);
        if(cocobot_trajectory_wait() != COCOBOT_TRAJECTORY_SUCCESS)
        {
            break;
        }
#ifdef AUSBEE_SIM
        try -= 1;
        if(try == 0)
        {
            break;
        }
#endif
    }
    meca_bee_action();

    vTaskDelay(1000/portTICK_PERIOD_MS);
    cocobot_position_set_y(-910);

    cocobot_trajectory_goto_d(-250, 5000);
    cocobot_game_state_add_points_to_score(50);
    cocobot_trajectory_wait();
    return COCOBOT_RETURN_ACTION_SUCCESS;
}

void strat_bee_register(void)
{
    cocobot_action_scheduler_add_action(
            "get_bee",
            strat_bee_get_score(),
            strat_bee_pos,
            strat_bee_get_exec_time(),
            strat_bee_get_success_proba(),
            strat_bee_preexec,
            strat_bee_exec,
            strat_bee_cleanup,
            NULL,
            NULL);
}
