#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"

static cocobot_arm_t arm[4] = {0};

void cocobot_arm_init(void)
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

  // TODO: replace by real current servo angles
  cocobot_arm_move_arti(&arm[0], 0, 0, 0, 0);
  cocobot_arm_print_pos(&arm[0]);
  cocobot_arm_move_arti(&arm[1], 90, 0, 0, 0);
  cocobot_arm_print_pos(&arm[1]);
  cocobot_arm_move_arti(&arm[2], 180, 0, 0, 0);
  cocobot_arm_print_pos(&arm[2]);
  cocobot_arm_move_arti(&arm[3], 270, 0, 0, 0);
  cocobot_arm_print_pos(&arm[3]);
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
