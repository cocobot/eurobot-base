#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_INVERSE_KINEMATICS

#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>

#define DEG2RAD(a) ((a) * M_PI / 180)
#define RAD2DEG(a) ((a) * 180 / M_PI)
#define SQUARE(x)  ((x) * (x))

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
    float a1Min_deg; float a1Max_deg;
    float a2Min_deg; float a2Max_deg;
    float a3Min_deg; float a3Max_deg;
    float a4Min_deg; float a4Max_deg;

    float xMin; float xMax;
    float yMin; float yMax;
    float zMin; float zMax;
    float alphaMin_deg; float alphaMax_deg;
} cocobot_kinematics_limits;

void cocobot_kinematics_init_DH_parameters(float d1, float r1, float r2, float r3, float r4, float e4)
{
    // Paramètres géométriques robot
    cocobot_kinematics.d1 = d1;
    cocobot_kinematics.r1 = r1;
    cocobot_kinematics.r2 = r2;
    cocobot_kinematics.r3 = r3;
    cocobot_kinematics.r4 = r4;
    cocobot_kinematics.e4 = e4; // (décallage ventouse en "y" par rapport aux axes du bras)

    // Positions limites articulations robot (en deg)
    cocobot_kinematics_limits.a1Min_deg=-180; cocobot_kinematics_limits.a1Max_deg=180;
    cocobot_kinematics_limits.a2Min_deg=- 90; cocobot_kinematics_limits.a2Max_deg= 90;
    cocobot_kinematics_limits.a3Min_deg=-160; cocobot_kinematics_limits.a3Max_deg=160;
    cocobot_kinematics_limits.a4Min_deg=- 90; cocobot_kinematics_limits.a4Max_deg= 90;

    // Positions limites commande effecteur (en m)
    cocobot_kinematics_limits.xMin= 0.000;   cocobot_kinematics_limits.xMax=0.3337;
    cocobot_kinematics_limits.yMin=-0.370;   cocobot_kinematics_limits.yMax=0.370;
    cocobot_kinematics_limits.zMin= 0.000;   cocobot_kinematics_limits.zMax=0.370;
    cocobot_kinematics_limits.alphaMin_deg=-180; cocobot_kinematics_limits.alphaMax_deg=180;
}

void cocobot_kinematics_set_angle_limits(float a1Min_deg, float a1Max_deg, float a2Min_deg, float a2Max_deg, float a3Min_deg, float a3Max_deg, float a4Min_deg, float a4Max_deg)
{
    cocobot_kinematics_limits.a1Min_deg=a1Min_deg; cocobot_kinematics_limits.a1Max_deg=a1Max_deg;
    cocobot_kinematics_limits.a2Min_deg=a2Min_deg; cocobot_kinematics_limits.a2Max_deg=a2Max_deg;
    cocobot_kinematics_limits.a3Min_deg=a3Min_deg; cocobot_kinematics_limits.a3Max_deg=a3Max_deg;
    cocobot_kinematics_limits.a4Min_deg=a4Min_deg; cocobot_kinematics_limits.a4Max_deg=a4Max_deg;
}

void cocobot_kinematics_set_cartesian_limits(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, float alphaMin_deg, float alphaMax_deg)
{
    cocobot_kinematics_limits.xMin=xMin;         cocobot_kinematics_limits.xMax=xMax;
    cocobot_kinematics_limits.yMin=yMin;         cocobot_kinematics_limits.yMax=yMax;
    cocobot_kinematics_limits.zMin=zMin;         cocobot_kinematics_limits.zMax=zMax;
    cocobot_kinematics_limits.alphaMin_deg=alphaMin_deg; cocobot_kinematics_limits.alphaMax_deg=alphaMax_deg;
}

void cocobot_kinematics_compute_forward(const cocobot_joint_pos_t * current_joint_pos, cocobot_cartesian_pos_t * new_cartesian_pos)
{
    float a23_deg = current_joint_pos->a2_deg + current_joint_pos->a3_deg;

    new_cartesian_pos->alpha_deg = current_joint_pos->a2_deg + current_joint_pos->a3_deg + current_joint_pos->a4_deg;


    float C1 = cocobot_kinematics.r2 * cosf(DEG2RAD(current_joint_pos->a2_deg))
             + cocobot_kinematics.r3 * cosf(DEG2RAD(a23_deg))
             + cocobot_kinematics.r4 * cosf(DEG2RAD(new_cartesian_pos->alpha_deg));

    new_cartesian_pos->x = cosf(DEG2RAD(current_joint_pos->a1_deg)) * (cocobot_kinematics.r1 + C1)
                         - sinf(DEG2RAD(current_joint_pos->a1_deg)) * cocobot_kinematics.e4;

    new_cartesian_pos->y = sinf(DEG2RAD(current_joint_pos->a1_deg)) * (cocobot_kinematics.r1 + C1)
                         + cosf(DEG2RAD(current_joint_pos->a1_deg)) * cocobot_kinematics.e4;

    new_cartesian_pos->z = cocobot_kinematics.d1
                         - cocobot_kinematics.r2 * sinf(DEG2RAD(current_joint_pos->a2_deg))
                         - cocobot_kinematics.r3 * sinf(DEG2RAD(a23_deg))
                         - cocobot_kinematics.r4 * sinf(DEG2RAD(new_cartesian_pos->alpha_deg));
}

void cocobot_kinematics_compute_inverse(const cocobot_cartesian_pos_t * current_cartesian_pos, cocobot_joint_pos_t * new_joint_pos)
{
    float r_target = sqrtf(SQUARE(current_cartesian_pos->x) + SQUARE(current_cartesian_pos->y));

    new_joint_pos->a1_deg = RAD2DEG(atan2f(current_cartesian_pos->y, current_cartesian_pos->x)
                                  - atan2f(cocobot_kinematics.e4,r_target));

    new_joint_pos->a3_deg = RAD2DEG(acosf(
                (SQUARE(r_target - cocobot_kinematics.r1
                      - cocobot_kinematics.r4 * cosf(DEG2RAD(current_cartesian_pos->alpha_deg)))
               + SQUARE(cocobot_kinematics.d1 - current_cartesian_pos->z
                      - cocobot_kinematics.r4 * sinf(DEG2RAD(current_cartesian_pos->alpha_deg)))
               - SQUARE(cocobot_kinematics.r2)
               - SQUARE(cocobot_kinematics.r3))
              / (2*cocobot_kinematics.r2*cocobot_kinematics.r3)));

    new_joint_pos->a2_deg = RAD2DEG(
            atan2f((cocobot_kinematics.d1 - current_cartesian_pos->z
                  - cocobot_kinematics.r4 * sinf(DEG2RAD(current_cartesian_pos->alpha_deg))),
                   (r_target - cocobot_kinematics.r1
                  - cocobot_kinematics.r4 * cosf(DEG2RAD(current_cartesian_pos->alpha_deg))))
          - atan2f((cocobot_kinematics.r3 * sinf(DEG2RAD(new_joint_pos->a3_deg))),
                   (cocobot_kinematics.r2 + cocobot_kinematics.r3*cosf(DEG2RAD(new_joint_pos->a3_deg)))));

    new_joint_pos->a4_deg = current_cartesian_pos->alpha_deg - new_joint_pos->a2_deg - new_joint_pos->a3_deg;
}

void cocobot_kinematics_print_joint_pos(const cocobot_joint_pos_t * joint_pos)
{
    printf("a1_deg=%.3f, a2_deg=%.3f, a3_deg=%.3f, a4_deg=%.3f\n", joint_pos->a1_deg, joint_pos->a2_deg, joint_pos->a3_deg, joint_pos->a4_deg);
}

void cocobot_kinematics_print_cartesian_pos(const cocobot_cartesian_pos_t * cartesian_pos)
{
    printf("x=%.3f, y=%.3f, z=%.3f, alpha_deg=%.3f\n", cartesian_pos->x, cartesian_pos->y, cartesian_pos->z, cartesian_pos->alpha_deg);
}

#endif
