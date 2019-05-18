#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"

static cocobot_arm_t arm[4] = {0};

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

  // TODO: replace by real current servo angles
  cocobot_arm_init(&arm[0], 0, 0, 0, 0);
  cocobot_arm_print_pos(&arm[0]);
  cocobot_arm_init(&arm[1], 90, 0, 0, 0);
  cocobot_arm_print_pos(&arm[1]);
  cocobot_arm_init(&arm[2], 180, 0, 0, 0);
  cocobot_arm_print_pos(&arm[2]);
  cocobot_arm_init(&arm[3], 270, 0, 0, 0);
  cocobot_arm_print_pos(&arm[3]);
}
