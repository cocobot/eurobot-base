#ifndef COCOBOT_ARM_H
#define COCOBOT_ARM_H

typedef struct
{
    cocobot_joint_pos_t current_joint_pos;
    cocobot_cartesian_pos_t current_cartesian_pos;
} cocobot_arm_t;

void cocobot_arm_init(void);
void cocobot_arm_move_arti(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg);
void cocobot_arm_move_cartesian(cocobot_arm_t * arm_ref, float x, float y, float z, float alpha_deg);
void cocobot_arm_print_pos(cocobot_arm_t * arm_ref);
void cocobot_arm_test_and_print(void);

#endif /* COCOBOT_ARM_H */
