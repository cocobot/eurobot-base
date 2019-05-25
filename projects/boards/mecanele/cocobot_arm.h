#ifndef COCOBOT_ARM_H
#define COCOBOT_ARM_H

typedef struct
{
    cocobot_joint_pos_t current_joint_pos;
    cocobot_cartesian_pos_t current_cartesian_pos;
    void (*get_current_servo_angles_function)(cocobot_joint_pos_t *);
    void (*update_servo_angles_function)(const cocobot_joint_pos_t *);
} cocobot_arm_t;

/* Warning: cocobot_kinematics_init_DH_parameters should be used before any of
 * the following functions */
void cocobot_arm_init(cocobot_arm_t * arm_ref,
                      void (*get_current_servo_angles_function)(cocobot_joint_pos_t *),
                      void (*update_servo_angles_function)(const cocobot_joint_pos_t *));
void cocobot_arm_move_arti(cocobot_arm_t * arm_ref, float a1_deg, float a2_deg, float a3_deg, float a4_deg);
void cocobot_arm_move_cartesian(cocobot_arm_t * arm_ref, float x, float y, float z, float alpha_deg);
void cocobot_arm_print_pos(cocobot_arm_t * arm_ref);
void cocobot_arm_test_and_print(void);

#endif /* COCOBOT_ARM_H */
