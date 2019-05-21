#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"

void cocobot_arm_init(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg)
{
  arm_ref->current_joint_pos.a1_deg = a1_deg;
  arm_ref->current_joint_pos.a2_deg = a2_deg;
  arm_ref->current_joint_pos.a3_deg = a3_deg;
  arm_ref->current_joint_pos.a4_deg = a4_deg;

  // Update cartesian position
  cocobot_kinematics_compute_forward(&(arm_ref->current_joint_pos), &(arm_ref->current_cartesian_pos));
}

void cocobot_arm_move_arti(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg)
{
  arm_ref->current_joint_pos.a1_deg = a1_deg;
  arm_ref->current_joint_pos.a2_deg = a2_deg;
  arm_ref->current_joint_pos.a3_deg = a3_deg;
  arm_ref->current_joint_pos.a4_deg = a4_deg;

  // TODO: send command to update servo angles

  // Update cartesian position
  cocobot_kinematics_compute_forward(&(arm_ref->current_joint_pos), &(arm_ref->current_cartesian_pos));
}

void cocobot_arm_move_cartesian(cocobot_arm_t * arm_ref, float x, float y, float z, float alpha_deg)
{
  arm_ref->current_cartesian_pos.x = x;
  arm_ref->current_cartesian_pos.y = y;
  arm_ref->current_cartesian_pos.z = z;
  arm_ref->current_cartesian_pos.alpha_deg = alpha_deg;

  // Update joint position
  cocobot_kinematics_compute_inverse(&(arm_ref->current_cartesian_pos), &(arm_ref->current_joint_pos));

  // TODO: send command to update servo angles
}

void cocobot_arm_print_pos(cocobot_arm_t * arm_ref)
{
  printf("Print arm : %p\n", arm_ref);
  printf("\tJoint pos :\n\t");
  cocobot_kinematics_print_joint_pos(&(arm_ref->current_joint_pos));
  printf("\tCartesian pos :\n\t");
  cocobot_kinematics_print_cartesian_pos(&(arm_ref->current_cartesian_pos));
}

void cocobot_arm_test_and_print(void)
{
  printf("\n==========ARM ACTION TEST PRINT==========\n");

  cocobot_arm_t arm_test = {0};
  printf("\n-------------------- Init:\n");
  cocobot_arm_print_pos(&arm_test);
  printf("\n-------------------- Move arti (0, 0, 0, 0):\n");
  cocobot_arm_move_arti(&arm_test, 0, 0, 0, 0);
  cocobot_arm_print_pos(&arm_test);
  printf("\n-------------------- Move arti (-15, -30, 75, -10):\n");
  cocobot_arm_move_arti(&arm_test, -15, -30, 75, -10);
  cocobot_arm_print_pos(&arm_test);
  printf("\n-------------------- Move cart (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_move_cartesian(&arm_test, 0.23, -0.023, 0.13, -42);
  cocobot_arm_print_pos(&arm_test);

  printf("==========END ARM ACTION TEST PRINT==========\n\n");
}