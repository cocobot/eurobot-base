#include <cocobot.h>
#include <stdio.h>
#include <cocobot_arm_action.h>

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
}

void cocobot_arm_action_move_arti(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg)
{
  arm_ref->current_joint_pos.a1_deg = a1_deg;
  arm_ref->current_joint_pos.a2_deg = a2_deg;
  arm_ref->current_joint_pos.a3_deg = a3_deg;
  arm_ref->current_joint_pos.a4_deg = a4_deg;

  // Update cartesian position
  cocobot_kinematics_compute_forward(&(arm_ref->current_joint_pos), &(arm_ref->current_cartesian_pos));
}

void cocobot_arm_action_move_cartesian(cocobot_arm_t * arm_ref, float x, float y, float z, float alpha_deg)
{
  arm_ref->current_cartesian_pos.x = x;
  arm_ref->current_cartesian_pos.y = y;
  arm_ref->current_cartesian_pos.z = z;
  arm_ref->current_cartesian_pos.alpha_deg = alpha_deg;

  // Update joint position
  cocobot_kinematics_compute_inverse(&(arm_ref->current_cartesian_pos), &(arm_ref->current_joint_pos));
}

void cocobot_arm_print_pos(cocobot_arm_t * arm_ref)
{
  cocobot_kinematics_print_joint_pos(&(arm_ref->current_joint_pos));
  cocobot_kinematics_print_cartesian_pos(&(arm_ref->current_cartesian_pos));
}

void cocobot_arm_action_test_print(void)
{
  printf("\n==========ARM ACTION TEST PRINT==========\n");
  cocobot_joint_pos_t joint_pos = { .a1_deg = 0, .a2_deg = 0, .a3_deg = 0, .a4_deg = 0 };
  cocobot_cartesian_pos_t cartesian_pos;

  cocobot_kinematics_compute_forward(&joint_pos, &cartesian_pos);

  printf("Joint pos :\n");
  cocobot_kinematics_print_joint_pos(&joint_pos);

  printf("Cartesian pos :\n");
  cocobot_kinematics_print_cartesian_pos(&cartesian_pos);

  joint_pos.a1_deg = -15;
  joint_pos.a2_deg = -30;
  joint_pos.a3_deg = 75;
  joint_pos.a4_deg = -10;

  cocobot_kinematics_compute_forward(&joint_pos, &cartesian_pos);

  printf("-------------------------------\n");
  printf("Joint pos :\n");
  cocobot_kinematics_print_joint_pos(&joint_pos);

  printf("Cartesian pos :\n");
  cocobot_kinematics_print_cartesian_pos(&cartesian_pos);

  cartesian_pos.x = 0.23;
  cartesian_pos.y = -0.023;
  cartesian_pos.z = 0.13;
  cartesian_pos.alpha_deg = -42;

  cocobot_kinematics_compute_inverse(&cartesian_pos, &joint_pos);

  printf("-------------------------------\n");
  printf("Cartesian pos :\n");
  cocobot_kinematics_print_cartesian_pos(&cartesian_pos);

  printf("Joint pos :\n");
  cocobot_kinematics_print_joint_pos(&joint_pos);
  printf("==========END ARM ACTION TEST PRINT==========\n\n");
}
