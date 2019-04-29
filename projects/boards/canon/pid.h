#ifndef _MOTOR_PID_H
#define _MOTOR_PID_H

#include <stdint.h>

int pid_is_limited(void);
void pid_set(float kp, float ki, float kd);
void pid_set_limit(float speed_limit, float quad_limit);
void pid_reset(void);
float pid_update(float velocity, uint64_t dt);
void pid_set_cons(float rpm);

#endif

