#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_INVERSE_KINEMATICS

#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>

static struct cocobot_kinematics_t
{
  float d1;
  float r1;
  float r2;
  float r3;
  float r4;
  float e4;
} cocobot_kinematics;

static struct cocobot_kinematics_limits_t
{
    float a1Min; float a1Max;
    float a2Min; float a2Max;
    float a3Min; float a3Max;
    float a4Min; float a4Max;

    float xMin; float xMax;
    float yMin; float yMax;
    float zMin; float zMax;
    float alphaMin; float alphaMax;
} cocobot_kinematics_limits;

void cocobot_inverse_kinematics_init_DH_parameters(float d1, float r1, float r2, float r3, float r4, float e4)
{
    // Paramètres géométriques robot
    cocobot_kinematics.d1 = d1;
    cocobot_kinematics.r1 = r1;
    cocobot_kinematics.r2 = r2;
    cocobot_kinematics.r3 = r3;
    cocobot_kinematics.r4 = r4;
    cocobot_kinematics.e4 = e4;

    // Positions limites articulations robot (en deg)
    cocobot_kinematics_limits.a1Min=-180; cocobot_kinematics_limits.a1Max=180;
    cocobot_kinematics_limits.a2Min=- 90; cocobot_kinematics_limits.a2Max= 90;
    cocobot_kinematics_limits.a3Min=-160; cocobot_kinematics_limits.a3Max=160;
    cocobot_kinematics_limits.a4Min=- 90; cocobot_kinematics_limits.a4Max= 90;

    // Positions limites commande effecteur (en m)
    cocobot_kinematics_limits.xMin= 0.000;   cocobot_kinematics_limits.xMax=0.3337;
    cocobot_kinematics_limits.yMin=-0.370;   cocobot_kinematics_limits.yMax=0.370;
    cocobot_kinematics_limits.zMin= 0.000;   cocobot_kinematics_limits.zMax=0.370;
    cocobot_kinematics_limits.alphaMin=-180; cocobot_kinematics_limits.alphaMax=180;
}

void cocobot_inverse_kinematics_set_angle_limits(float a1Min, float a1Max, float a2Min, float a2Max, float a3Min, float a3Max, float a4Min, float a4Max)
{
    cocobot_kinematics_limits.a1Min=a1Min; cocobot_kinematics_limits.a1Max=a1Max;
    cocobot_kinematics_limits.a2Min=a2Min; cocobot_kinematics_limits.a2Max=a2Max;
    cocobot_kinematics_limits.a3Min=a3Min; cocobot_kinematics_limits.a3Max=a3Max;
    cocobot_kinematics_limits.a4Min=a4Min; cocobot_kinematics_limits.a4Max=a4Max;
}

void cocobot_inverse_kinematics_set_cartesian_limits(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, float alphaMin, float alphaMax)
{
    cocobot_kinematics_limits.xMin=xMin;         cocobot_kinematics_limits.xMax=xMax;
    cocobot_kinematics_limits.yMin=yMin;         cocobot_kinematics_limits.yMax=yMax;
    cocobot_kinematics_limits.zMin=zMin;         cocobot_kinematics_limits.zMax=zMax;
    cocobot_kinematics_limits.alphaMin=alphaMin; cocobot_kinematics_limits.alphaMax=alphaMax;
}

void cocobot_inverse_kinematics_compute_forward_kinematics(const cocobot_joint_pos_t * current_joint_pos, cocobot_cartesian_value_t * new_cartesian_value)
{
    float a23 = current_joint_pos->a2 + current_joint_pos->a3;

    new_cartesian_value->alpha = current_joint_pos->a2 + current_joint_pos->a3 + current_joint_pos->a4;


    float C1 = cocobot_kinematics.r2 * cosf(current_joint_pos->a2)
        + cocobot_kinematics.r3 * cosf(a23)
        + cocobot_kinematics.r4 * cosf(new_cartesian_value->alpha);

    new_cartesian_value->x = cosf(current_joint_pos->a1) * (cocobot_kinematics.r1 + C1)
                           - sinf(current_joint_pos->a1) * cocobot_kinematics.e4;

    new_cartesian_value->y = sinf(current_joint_pos->a1) * (cocobot_kinematics.r1 + C1)
                           + cosf(current_joint_pos->a1) * cocobot_kinematics.e4;

    new_cartesian_value->z = cocobot_kinematics.d1
                           - cocobot_kinematics.r2 * sinf(current_joint_pos->a2)
                           - cocobot_kinematics.r3 * sinf(a23)
                           - cocobot_kinematics.r4 * sinf(new_cartesian_value->alpha);
}

//void cocobot_inverse_kinematics_compute(const cocobot_cartesian_value_t * current_cartesian_value, cocobot_joint_pos_t * new_joint_pos)
//{
//}

#endif
