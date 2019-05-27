#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void servo_init(void);
float servo_get_angle(unsigned int servo_id);
void servo_set_angle(unsigned int servo_id, float angle);
void servo_set_pwm(unsigned int servo_id, uint16_t pwm);

#endif// SERVO_H
