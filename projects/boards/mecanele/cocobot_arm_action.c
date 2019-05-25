#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"

static cocobot_arm_t arm[4] = {0};

static void arm0_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
  // TODO: Replace with function to get servo angle
  // joint_pos->a1_deg = get_servo_angle(servo_id);
  joint_pos->a1_deg = 0;
  joint_pos->a2_deg = 10;
  joint_pos->a3_deg = 20;
  joint_pos->a4_deg = 30;
}

static void arm0_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
  // TODO: Replace with function to set servo angle
  // set_servo_angle(servo_id, joint_pos->a1_deg);
  printf("TODO: set servo 1 angle to %f\n", joint_pos->a1_deg);
  printf("TODO: set servo 2 angle to %f\n", joint_pos->a2_deg);
  printf("TODO: set servo 3 angle to %f\n", joint_pos->a3_deg);
  printf("TODO: set servo 4 angle to %f\n", joint_pos->a4_deg);
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

  cocobot_arm_init(&arm[0], &arm0_get_current_servo_angles_function, &arm0_update_servo_angles_function);
  // TODO: Do the same for arm1, arm2 and arm3
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
