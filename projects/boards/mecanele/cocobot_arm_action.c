#include <cocobot.h>
#include <stdio.h>
#include "servo.h"
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"

static cocobot_arm_t arm[4] = {0};

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
  servo_set_angle(0, joint_pos->a1_deg); 
  servo_set_angle(1, joint_pos->a1_deg); 
  servo_set_angle(2, joint_pos->a1_deg); 
}

static void arm1_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
  servo_set_angle(12, joint_pos->a1_deg); 
  servo_set_angle(3, joint_pos->a1_deg); 
  servo_set_angle(4, joint_pos->a1_deg); 
  servo_set_angle(5, joint_pos->a1_deg); 
}

static void arm2_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
  servo_set_angle(12, joint_pos->a1_deg); 
  servo_set_angle(6, joint_pos->a1_deg); 
  servo_set_angle(7, joint_pos->a1_deg); 
  servo_set_angle(8, joint_pos->a1_deg); 
}

static void arm3_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
  servo_set_angle(12, joint_pos->a1_deg); 
  servo_set_angle(9, joint_pos->a1_deg); 
  servo_set_angle(10, joint_pos->a1_deg); 
  servo_set_angle(11, joint_pos->a1_deg); 
}

void cocobot_arm_action_init(void)
{
  cocobot_kinematics_init_DH_parameters(0.0971, 0.0637, 0.150, 0.080, 0.040, -0.0117);

  cocobot_kinematics_set_angle_limits(-180, 180,
                                      - 90,  90,
                                      -160, 160,
                                      - 90,  90);

  cocobot_kinematics_set_cartesian_limits(0.000, 0.3337,
                                         -0.370, 0.37,
                                          0.000, 0.37,
                                           -180,  180);

  // Front arm
  cocobot_arm_init(&arm[0], &arm0_get_current_servo_angles_function, &arm0_update_servo_angles_function);
  // Left arm
  cocobot_arm_init(&arm[1], &arm1_get_current_servo_angles_function, &arm1_update_servo_angles_function);
  cocobot_arm_set_direction(&arm[1], 90);
  // Back arm
  cocobot_arm_init(&arm[2], &arm2_get_current_servo_angles_function, &arm2_update_servo_angles_function);
  cocobot_arm_set_direction(&arm[2], 180);
  // Right arm
  cocobot_arm_init(&arm[3], &arm3_get_current_servo_angles_function, &arm3_update_servo_angles_function);
  cocobot_arm_set_direction(&arm[3], -90);
}

void cocobot_arm_action_prendre_palais_sol(int arm_id, float x, float y)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_cartesian(arm[arm_id], x, y, z, alpha_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
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
