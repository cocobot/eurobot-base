#ifndef COCOBOT_INVERSE_KINEMATICS_H
#define COCOBOT_INVERSE_KINEMATICS_H

typedef struct
{
    float a1_deg;
    float a2_deg;
    float a3_deg;
    float a4_deg;
} cocobot_joint_pos_t;

typedef struct
{
    float x;
    float y;
    float z;
    float alpha_deg;
} cocobot_cartesian_pos_t;

/* Set Denavit–Hartenberg for inverse kinematics. Must be called once before any other function.
 */
void cocobot_kinematics_init_DH_parameters(float d1, float r1, float r2, float r3, float r4, float e4);

void cocobot_kinematics_set_angle_limits(float a1Min_deg, float a1Max_deg, float a2Min_deg, float a2Max_deg, float a3Min_deg, float a3Max_deg, float a4Min_deg, float a4Max_deg);

void cocobot_kinematics_set_cartesian_limits(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, float alphaMin_deg, float alphaMax_deg);

void cocobot_kinematics_compute_forward(const cocobot_joint_pos_t * current_joint_pos, cocobot_cartesian_pos_t * new_cartesian_pos);

void cocobot_kinematics_compute_inverse(const cocobot_cartesian_pos_t * current_cartesian_pos, cocobot_joint_pos_t * new_joint_pos);

void cocobot_kinematics_print_joint_pos(const cocobot_joint_pos_t * joint_pos);

void cocobot_kinematics_print_cartesian_pos(const cocobot_cartesian_pos_t * cartesian_pos);

#endif // COCOBOT_INVERSE_KINEMATICS_H
