#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"

void cocobot_arm_init(cocobot_arm_t * arm_ref,
                      void (*get_current_servo_angles_function)(cocobot_joint_pos_t *),
                      void (*update_servo_angles_function)(const cocobot_joint_pos_t *))
{
  arm_ref->get_current_servo_angles_function = get_current_servo_angles_function;
  arm_ref->update_servo_angles_function = update_servo_angles_function;

  arm_ref->arm_direction_deg = 0;

  // Set arm_ref->current_joint_pos to current servo angles
  (*(arm_ref->get_current_servo_angles_function))(&(arm_ref->current_joint_pos));

  // Update cartesian position
  cocobot_kinematics_compute_forward(&(arm_ref->current_joint_pos), &(arm_ref->current_cartesian_pos));
}

void cocobot_arm_set_direction(cocobot_arm_t * arm_ref, float direction_deg)
{
  arm_ref->arm_direction_deg = direction_deg;
}

void cocobot_arm_move_arti(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg)
{
  arm_ref->current_joint_pos.a1_deg = a1_deg;
  arm_ref->current_joint_pos.a2_deg = a2_deg;
  arm_ref->current_joint_pos.a3_deg = a3_deg;
  arm_ref->current_joint_pos.a4_deg = a4_deg;

  // Update cartesian position
  cocobot_kinematics_compute_forward(&(arm_ref->current_joint_pos), &(arm_ref->current_cartesian_pos));

  // Correction de l'orientation du plateau tournant en fonction du bras choisi
  arm_ref->current_joint_pos.a1_deg = arm_ref->current_joint_pos.a1_deg - arm_ref->arm_direction_deg;

  // Send command to update servo angles
  (*(arm_ref->update_servo_angles_function))(&(arm_ref->current_joint_pos));
}

void cocobot_arm_move_cartesian(cocobot_arm_t * arm_ref, float x, float y, float z, float alpha_deg)
{
  arm_ref->current_cartesian_pos.x = x;
  arm_ref->current_cartesian_pos.y = y;
  arm_ref->current_cartesian_pos.z = z;
  arm_ref->current_cartesian_pos.alpha_deg = alpha_deg;

  // Update joint position
  cocobot_kinematics_compute_inverse(&(arm_ref->current_cartesian_pos), &(arm_ref->current_joint_pos));

  // Correction de l'orientation du plateau tournant en fonction du bras choisi
  arm_ref->current_joint_pos.a1_deg = arm_ref->current_joint_pos.a1_deg - arm_ref->arm_direction_deg;

  // Send command to update servo angles
  (*(arm_ref->update_servo_angles_function))(&(arm_ref->current_joint_pos));
}

void cocobot_arm_print_pos(cocobot_arm_t * arm_ref)
{
  printf("Print arm : %p\n", arm_ref);
  printf("\tJoint pos :\n\t");
  cocobot_kinematics_print_joint_pos(&(arm_ref->current_joint_pos));
  printf("\tCartesian pos :\n\t");
  cocobot_kinematics_print_cartesian_pos(&(arm_ref->current_cartesian_pos));
}

static void arm_get_current_servo_angles_function(cocobot_joint_pos_t * joint_pos)
{
  // TODO: Replace with function to get servo angle
  // joint_pos->a1_deg = get_servo_angle(servo_id);
  joint_pos->a1_deg = 0;
  joint_pos->a2_deg = 10;
  joint_pos->a3_deg = 20;
  joint_pos->a4_deg = 30;
}

static void arm_update_servo_angles_function(const cocobot_joint_pos_t * joint_pos)
{
  // TODO: Replace with function to set servo angle
  // set_servo_angle(servo_id, joint_pos->a1_deg);
  cocobot_com_printf("TODO: set servo 1 angle to %f\n", (double) joint_pos->a1_deg);
  cocobot_com_printf("TODO: set servo 2 angle to %f\n", (double) joint_pos->a2_deg);
  cocobot_com_printf("TODO: set servo 3 angle to %f\n", (double) joint_pos->a3_deg);
  cocobot_com_printf("TODO: set servo 4 angle to %f\n", (double) joint_pos->a4_deg);
}

void cocobot_arm_test_and_print(void)
{
  printf("\n==========ARM ACTION TEST PRINT==========\n");

  cocobot_arm_t arm_test = {0};
  printf("\n-------------------- Init 0:\n");
  cocobot_arm_print_pos(&arm_test);
  printf("\n-------------------- Init function:\n");
  cocobot_arm_init(&arm_test, &arm_get_current_servo_angles_function, &arm_update_servo_angles_function);
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
