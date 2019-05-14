#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>

int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  cocobot_kinematics_init_DH_parameters(0.0971, 0.0637, 0.150, 0.080, 0.040, -0.0117);

  cocobot_kinematics_set_angle_limits(-180, 180,
                                              - 90,  90,
                                              -160, 160,
                                              - 90,  90);

  cocobot_kinematics_set_cartesian_limits(0.000, 0.3337,
                                                 -0.370, 0.37,
                                                  0.000, 0.37,
                                                   -180,  180);

  cocobot_joint_pos_t joint_pos = { .a1_deg = 0, .a2_deg = 0, .a3_deg = 0, .a4_deg = 0 };
  cocobot_cartesian_pos_t cartesian_pos;

  cocobot_kinematics_compute_forward(&joint_pos, &cartesian_pos);

  printf("-------------------------------\n");
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
  printf("-------------------------------\n");


  vTaskStartScheduler();

  return 0;
}
