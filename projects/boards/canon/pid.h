#ifndef _MOTOR_PID_H
#define _MOTOR_PID_H

#include <stdint.h>


void pid_set(float kp, float ki);
void pid_reset(void);
float pid_update(float velocity, uint64_t dt);
void pid_set_cons(float rpm);

#endif

