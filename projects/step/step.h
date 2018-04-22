#ifndef STEP_H
#define STEP_H

#include <stdint.h>

typedef struct
{
  uint32_t status;
  int32_t stepper_command[4];
  int32_t stepper_position[4];
} board_state_t;



#endif// STEP_H
