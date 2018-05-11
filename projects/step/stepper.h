#ifndef STEPPER_H
#define STEPPER_H
#include "step.h"

void stepper_init(void);
void stepper_home(int id);
void stepper_target(int id, int32_t t);
int stepper_is_busy(void);

#endif// STEPPER_H

