#include <cocobot.h>
#include <stdio.h>
#include "servo.h"
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"
#include "FreeRTOS.h"
#include "task.h"

static cocobot_arm_t arm[4] = {0};

#if 0
static void arm0_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
    joint_pos->a1_deg = servo_get_angle(12);
    joint_pos->a2_deg = servo_get_angle(0);
    joint_pos->a3_deg = servo_get_angle(1);
    joint_pos->a4_deg = servo_get_angle(2);
}

static void arm1_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
    joint_pos->a1_deg = servo_get_angle(12);
    joint_pos->a2_deg = servo_get_angle(3);
    joint_pos->a3_deg = servo_get_angle(4);
    joint_pos->a4_deg = servo_get_angle(5);
}

static void arm2_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
    joint_pos->a1_deg = servo_get_angle(12);
    joint_pos->a2_deg = servo_get_angle(6);
    joint_pos->a3_deg = servo_get_angle(7);
    joint_pos->a4_deg = servo_get_angle(8);
}

static void arm3_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
    joint_pos->a1_deg = servo_get_angle(12);
    joint_pos->a2_deg = servo_get_angle(9);
    joint_pos->a3_deg = servo_get_angle(10);
    joint_pos->a4_deg = servo_get_angle(11);
}

static void arm0_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
    servo_set_angle(12, joint_pos->a1_deg); 
    servo_set_angle(0,  joint_pos->a2_deg);
    servo_set_angle(1,  joint_pos->a3_deg);
    servo_set_angle(2,  joint_pos->a4_deg);
}

static void arm1_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
    servo_set_angle(12, joint_pos->a1_deg); 
    servo_set_angle(3,  joint_pos->a2_deg);
    servo_set_angle(4,  joint_pos->a3_deg);
    servo_set_angle(5,  joint_pos->a4_deg);
}

static void arm2_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
    servo_set_angle(12, joint_pos->a1_deg); 
    servo_set_angle(6,  joint_pos->a2_deg);
    servo_set_angle(7,  joint_pos->a3_deg);
    servo_set_angle(8,  joint_pos->a4_deg);
}

static void arm3_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
    servo_set_angle(12, joint_pos->a1_deg); 
    servo_set_angle(9,  joint_pos->a2_deg);
    servo_set_angle(10, joint_pos->a3_deg);
    servo_set_angle(11, joint_pos->a4_deg);
}
#endif
void cocobot_arm_action_init(void)
{
    //cocobot_kinematics_init_DH_parameters(0.0971, 0.0637, 0.150, 0.080, 0.040, -0.0117);

    //cocobot_kinematics_set_angle_limits(-180, 180,
    //                                    - 90,  90,
    //                                    -160, 160,
    //                                    - 90,  90);

    //cocobot_kinematics_set_cartesian_limits(0.000, 0.3337,
    //                                       -0.370, 0.37,
    //                                        0.000, 0.37,
    //                                         -180,  180);

    //// Front arm
    //cocobot_arm_init(&arm[0], &arm0_get_current_servo_angles_function, &arm0_update_servo_angles_function);
    //// Left arm
    //cocobot_arm_init(&arm[1], &arm1_get_current_servo_angles_function, &arm1_update_servo_angles_function);
    //cocobot_arm_set_direction(&arm[1], 90);
    //// Back arm
    //cocobot_arm_init(&arm[2], &arm2_get_current_servo_angles_function, &arm2_update_servo_angles_function);
    //cocobot_arm_set_direction(&arm[2], 180);
    //// Right arm
    //cocobot_arm_init(&arm[3], &arm3_get_current_servo_angles_function, &arm3_update_servo_angles_function);
    //cocobot_arm_set_direction(&arm[3], -90);

    for(int i = 0; i <= 3; i++)
    {
        cocobot_arm_action_repos_vide(i);
    }
}

void cocobot_arm_action_prendre_palais_sol(int arm_id, float x, float y)
{
    if(arm_id == 0)
    {
        servo_set_angle(2, 5);
        servo_set_angle(1, -110);
        servo_set_angle(0, 5);
        return; 
    }
    if(arm_id == 2)
    {
        servo_set_angle(8, 5);
        servo_set_angle(7, -110);
        servo_set_angle(6, 10);
        vTaskDelay(100/portTICK_PERIOD_MS);
        servo_set_angle(6, 10);

        return; 
    }
    if(arm_id == 3)
    {
        servo_set_angle(9, 30);
        servo_set_angle(11, -65);
        servo_set_angle(10, -20);

        vTaskDelay(500/portTICK_PERIOD_MS);
        servo_set_angle(9, -15);
        return; 
    }
    if(arm_id == 1)
    {
        servo_set_angle(3, 5);
        servo_set_angle(4, -70);
        servo_set_angle(5, 30);
        return; 
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
    servo_set_pwm(arm_id*3 + 2, 0);
}

void cocobot_arm_action_prise_sol_second_try(int arm_id, float x, float y)
{
    if(arm_id == 0)
    {
        servo_set_angle(0, 30);
        vTaskDelay(200/portTICK_PERIOD_MS);
        servo_set_angle(0, 10);
        return; 
    }
    if(arm_id == 2)
    {
        servo_set_angle(6, 30);
        vTaskDelay(200/portTICK_PERIOD_MS);
        servo_set_angle(6, 10);

        return; 
    }
    if(arm_id == 3)
    {
        servo_set_angle(9, 5);
        vTaskDelay(200/portTICK_PERIOD_MS);
        servo_set_angle(9, -15);
        return; 
    }
    if(arm_id == 1)
    {
        servo_set_angle(3, 25);
        vTaskDelay(200/portTICK_PERIOD_MS);
        servo_set_angle(3, 5);
        return; 
    }

}

void cocobot_arm_action_repos_vide(int arm_id)
{
    if(arm_id == 3)
    {
        servo_set_angle(arm_id*3, 80);
        vTaskDelay(100/portTICK_PERIOD_MS);
        servo_set_angle(arm_id*3 + 1, -140);
        vTaskDelay(100/portTICK_PERIOD_MS);
        servo_set_angle(arm_id*3 + 2, 0);
        vTaskDelay(100/portTICK_PERIOD_MS);
        return;
    }
    servo_set_angle(arm_id*3, 80);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 1, -150);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 2, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);
}



void cocobot_arm_action_repos_normal(int arm_id)
{
    cocobot_arm_action_repos_vide(arm_id);
}

void cocobot_arm_action_pos_stockage_palais(int arm_id)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_open_accelerateur(int arm_id, float angle)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
    if(arm_id == 3)
    {
        servo_set_angle(9, 55);
        servo_set_angle(10, -55);
        servo_set_angle(11, 0);
    }
    if(arm_id == 2)
    {
        servo_set_angle(6, 80);
        servo_set_angle(7, -60);
        servo_set_angle(8, -20);
    }
    else if(arm_id == 1)
    {
        servo_set_angle(3, 54);
        servo_set_angle(4, -55);
        servo_set_angle(5, 0);
    }
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }

}

void cocobot_arm_action_prise_distributeur(int arm_id, float angle)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_orientation(int arm_id, int side)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_prise_goldenium(int arm_id, float angle)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        if(arm_id == 3)
        {
            servo_set_angle(10, -75);
            servo_set_angle(11, 10);
            vTaskDelay(200/portTICK_PERIOD_MS);
            servo_set_angle(9, 50);
        }
        else if(arm_id == 1)
        {
            servo_set_angle(3, 50);
            servo_set_angle(4, -75);
            servo_set_angle(5, -10);
        }
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_repos_goldenium(int arm_id)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_prise_bluenium(int arm_id, float angle)
{
    servo_set_angle(7, -70);
    servo_set_angle(8, 20);

    vTaskDelay(500/portTICK_PERIOD_MS);

    servo_set_angle(6, 50);
}

void cocobot_arm_action_depose_balance(int arm_id, float angle)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_depose_accelerateur_particules(int arm_id, float angle, float distance)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_depose_case(int arm_id, float angle)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        servo_set_angle(arm_id*3, 50);
        servo_set_angle(arm_id*3 + 1, -80);
        servo_set_angle(arm_id*3 + 2, -30);
        // TODO: à régler !
        //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_move_arm(int arm_id, float x, float y, float z, float a)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        cocobot_arm_move_cartesian(&arm[arm_id], x, y, z, a);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_print_pos(int arm_id)
{
    if (arm_id >= 0 && arm_id <= 3)
    {
        cocobot_arm_print_pos(&arm[arm_id]);
    }
    else
    {
        printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
    }
}

void cocobot_arm_action_test_and_print(void)
{
    printf("\n-------------------- Move arm 0 arti (0, 0, 0, 0):\n");
    cocobot_arm_move_arti(&arm[0], 0, 0, 0, 0);
    cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 1 arti (0, 0, 0, 0):\n");
    cocobot_arm_move_arti(&arm[1], 0, 0, 0, 0);
    cocobot_arm_action_print_pos(1);
    printf("\n-------------------- Move arm 0 cart (0.3334, -0.012, 0.097, 0)\n");
    cocobot_arm_action_move_arm(0, 0.3334, -0.012, 0.097, 0);
    cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 1 cart (0.3334, -0.012, 0.097, 0)\n");
    cocobot_arm_action_move_arm(1, 0.3334, -0.012, 0.097, 0);
    cocobot_arm_action_print_pos(1);
    printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
    cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
    cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 1 cart  (0.23, -0.023, 0.13, -42)\n");
    cocobot_arm_action_move_arm(1, 0.23, -0.023, 0.13, -42);
    cocobot_arm_action_print_pos(1);
}

void cocobot_arm_action_test_multiple_move_arm(void)
{
    printf("========== TEST PRINT ==========\n\n");
    printf("\n-------------------- Move arm 0 arti (0, 0, 0, 0):\n");
    cocobot_arm_move_arti(&arm[0], 0, 0, 0, 0);
    //cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
    cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
    //cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
    cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
    //cocobot_arm_action_print_pos(0);
    printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
    cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
    //cocobot_arm_action_print_pos(0);
    printf("\n\n==========END TEST PRINT ==========\n\n");
}
