#ifndef COCOBOT_INVERSE_KINEMATICS_H
#define COCOBOT_INVERSE_KINEMATICS_H

typedef struct
{
    float a1;
    float a2;
    float a3;
    float a4;
} cocobot_joint_pos_t;

typedef struct
{
    float x;
    float y;
    float z;
    float alpha;
} cocobot_cartesian_value_t;

/* Set Denavit–Hartenberg for inverse kinematics. Must be called once before any other function.
 */
void cocobot_inverse_kinematics_init_DH_parameters(float d1, float r1, float r2, float r3, float r4, float e4);

void cocobot_inverse_kinematics_set_angle_limits(float a1Min, float a1Max, float a2Min, float a2Max, float a3Min, float a3Max, float a4Min, float a4Max);

void cocobot_inverse_kinematics_set_cartesian_limits(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, float alphaMin, float alphaMax);

void cocobot_inverse_kinematics_compute_forward_kinematics(const cocobot_joint_pos_t * current_joint_pos, cocobot_cartesian_value_t * new_cartesian_value);

void cocobot_inverse_kinematics_compute(const cocobot_cartesian_value_t * current_cartesian_value, cocobot_joint_pos_t * new_joint_pos);

#endif // COCOBOT_INVERSE_KINEMATICS_H
