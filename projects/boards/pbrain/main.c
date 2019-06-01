#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "meca.h"

#define DEMI_LARGEUR_ROBOT      150
#define DEMI_LONGUEUR_ARR_ROBOT 107
#define MARGE_PRISE_BRAS        100
#define MARGE_DEPOSE_BRAS       100


extern cocobot_pathfinder_table_init_s initTable [];

static void stop_meca(void)
{
    meca_action(0, MECA_RELEASE);
}

static void cocobot_callage_backward()
{
    cocobot_trajectory_result_t result;
    //no angle
    cocobot_asserv_set_angle_activation(0);
    //move backard until it is blocked
    do
    {
        cocobot_trajectory_goto_d(-20, 2000);
        result = cocobot_trajectory_wait();
    }
    while(result == COCOBOT_TRAJECTORY_SUCCESS); 

}

void stop(void)
{
    cocobot_trajectory_wait();
    cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void run_homologation_pmi(void * arg)
{
    (void)arg;
    cocobot_game_state_wait_for_configuration();
    cocobot_game_state_wait_for_starter_removed();
    cocobot_trajectory_goto_d(300, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();
    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void run_homologation(void * arg)
{
    (void)arg;
    cocobot_game_state_wait_for_configuration();

    switch(cocobot_game_state_get_color())
    {
        case COCOBOT_GAME_STATE_COLOR_NEG:
            cocobot_position_set_x(-1180);
            cocobot_position_set_y(550);
            cocobot_position_set_angle(0);
            break;

        case COCOBOT_GAME_STATE_COLOR_POS:
            cocobot_position_set_x(1180);
            cocobot_position_set_y(550);
            cocobot_position_set_angle(180);
            break;
    }

    cocobot_game_state_wait_for_starter_removed();
    //sort de la zone vers l'experience
    cocobot_trajectory_goto_d(150, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();

    //tourne face a l'autre equipe
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_a(180, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }

    cocobot_trajectory_wait();

    //Avance de 20cm
    cocobot_trajectory_goto_d(300, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();

    //tourne face à la balance
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    cocobot_trajectory_wait();

    //Avance de 20cm
    cocobot_trajectory_goto_d(200, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();

    //tourne face à la balance
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_a(180, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    cocobot_trajectory_wait();

    //Avance de 30cm
    cocobot_trajectory_goto_d(300, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();

    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void run_strat(void * arg)
{
    (void)arg;

    cocobot_game_state_wait_for_configuration();

    //set initial position

    switch(cocobot_game_state_get_color())
    {
        case COCOBOT_GAME_STATE_COLOR_NEG:
            cocobot_position_set_x(-1180);
            cocobot_position_set_y(550);
            cocobot_position_set_angle(0);
            break;

        case COCOBOT_GAME_STATE_COLOR_POS:
            cocobot_position_set_x(1180);
            cocobot_position_set_y(550);
            cocobot_position_set_angle(180);
            break;
    }

    //pose exp
    cocobot_game_state_add_points_to_score(5);
    cocobot_game_state_wait_for_starter_removed();

    //electron
    cocobot_game_state_add_points_to_score(35);

    //saisir palet juste devant la zone rouge
    meca_action(0, MECA_TAKE_FLOOR);

    //palais zone verte
    cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_xy(-980, 550, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_xy_backward(1000, 550, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    cocobot_trajectory_wait();
    meca_action(3, MECA_TAKE_FLOOR);

    //palet zone bleue
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_xy(-991, 160, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_xy_backward(1000, 160, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }

    cocobot_trajectory_wait();
    meca_action(2, MECA_TAKE_FLOOR);  

    //retour case rouge et depose des 3 bras.
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
        cocobot_trajectory_goto_xy(-1080, 560, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    else
        cocobot_trajectory_goto_xy(1080, 560, COCOBOT_TRAJECTORY_UNLIMITED_TIME);

    int i;
    for(i = 0; i < 4; i += 1)
    {
        cocobot_trajectory_goto_a(-i * 90 + 90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();

        if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
            meca_action((i + 1)%4, MECA_DROP_FLOOR);
        else
            meca_action((i + 3)%4, MECA_DROP_FLOOR);

    }
    cocobot_game_state_add_points_to_score(1 + 6 + 6);

    //Activation de l'accelerateur
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_xy(300, 700, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(2, MECA_PUSH_ACCEL);
        cocobot_callage_backward();
        cocobot_position_set_angle(-90);
        cocobot_position_set_y(870);
        cocobot_trajectory_goto_d(100, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        //angle is back
        cocobot_asserv_set_angle_activation(1);
        cocobot_trajectory_goto_xy(230, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(2, MECA_REST_EMPTY);
        meca_action(3, MECA_PUSH_ACCEL);
        cocobot_trajectory_goto_a(180, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_xy_backward(500, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }
    else
    {
        cocobot_trajectory_goto_xy(-300, 700, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(2, MECA_PUSH_ACCEL);
        cocobot_callage_backward();
        cocobot_position_set_angle(-90);
        cocobot_position_set_y(870);
        cocobot_trajectory_goto_d(100, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        //angle is back
        cocobot_asserv_set_angle_activation(1);
        cocobot_trajectory_goto_xy(-230, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(2, MECA_REST_EMPTY);
        meca_action(1, MECA_PUSH_ACCEL);
        cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_xy_backward(-500, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    }

    cocobot_trajectory_wait();
    cocobot_game_state_add_points_to_score(20);

    //goldenium
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
        cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(1, MECA_REST_EMPTY);
        cocobot_trajectory_goto_xy_backward(710, 690, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(180, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(3, MECA_TAKE_GOLD);
        cocobot_trajectory_goto_a(90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
    }
    else
    {
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(1, MECA_REST_EMPTY);
        cocobot_trajectory_goto_xy_backward(-710, 690, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_a(0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
        meca_action(1, MECA_TAKE_GOLD);
        cocobot_trajectory_goto_a(-90, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_wait();
    }

    cocobot_game_state_add_points_to_score(20);

    //bring back golden
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
    }
    else
    {
        cocobot_trajectory_goto_xy(0, 215, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        cocobot_trajectory_goto_xy_backward(272, -600 + DEMI_LARGEUR_ROBOT + MARGE_DEPOSE_BRAS, 4000);
        cocobot_trajectory_wait();

    }




    //if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    //{
    //    cocobot_trajectory_goto_a(0, 2000);
    //    cocobot_trajectory_goto_xy(386, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    //}
    //else
    //{
    //    cocobot_trajectory_goto_a(180, 2000);
    //    cocobot_trajectory_goto_xy(-386, 740, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    //}





    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }



    //////DEBUG//////
    while(1)
    {
        //do nothing !
        uprintf("x=%d y=%d a=%d d=%d -- l=%ld r=%ld\r\n", 
                (int)cocobot_position_get_x(),
                (int)cocobot_position_get_y(),
                (int)cocobot_position_get_angle(),
                (int)cocobot_position_get_distance(),
                (int)cocobot_position_get_left_encoder(),
                (int)cocobot_position_get_right_encoder());

        cocobot_com_printf(COM_DEBUG, "x=%d y=%d a=%d d=%d -- l=%ld r=%ld\r\n", 
                (int)cocobot_position_get_x(),
                (int)cocobot_position_get_y(),
                (int)cocobot_position_get_angle(),
                (int)cocobot_position_get_distance(),
                (int)cocobot_position_get_left_encoder(),
                (int)cocobot_position_get_right_encoder());

        vTaskDelay(100/portTICK_PERIOD_MS);
    }

    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

int main(void) 
{
    platform_init();

    cocobot_com_init();
    cocobot_com_run();
    cocobot_position_init(4);
    cocobot_action_scheduler_init();
    cocobot_asserv_init();
    cocobot_trajectory_init(4);
    cocobot_opponent_detection_init(3);
    cocobot_game_state_init(stop_meca);
    cocobot_pathfinder_init(initTable);
    cocobot_action_scheduler_use_pathfinder(1);
    meca_init();

    cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);

    //set initial position
    cocobot_position_set_x(0);
    cocobot_position_set_y(0);
    cocobot_position_set_angle(0);

    xTaskCreate(run_strat, "strat", 600, NULL, 2, NULL );
    //xTaskCreate(run_homologation, "homolo", 600, NULL, 2, NULL );
    //xTaskCreate(run_homologation_pmi, "homolo_pmi", 600, NULL, 2, NULL );

    vTaskStartScheduler();

    return 0;
}
