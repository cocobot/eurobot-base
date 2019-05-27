#include <platform.h>
#include <stdint.h>
#include <math.h>
#include "servo.h"
#include "cocobot_config.h"

typedef struct 
{
  uint16_t setpoint_pwm;
} servo_t;

typedef struct 
{
  float angle_deg;
  float setpoint_pwm;
} servo_calibration_t;

static servo_t _servos[SERVO_NUMBER];
static servo_calibration_t _calibration[SERVO_NUMBER][2] = SERVO_CALIBRATION;

void servo_init(void)
{
  unsigned int i;

  for(i = 0; i < sizeof(_servos)/sizeof(_servos[0]); i += 1)
  {
    _servos[i].setpoint_pwm = 0;
  }
}

float servo_get_angle(unsigned int servo_id)
{
  float current_sp = _servos[servo_id].setpoint_pwm;

  if(current_sp == 0)
  {
    return NAN;
  }

  if(_calibration[servo_id][0].setpoint_pwm == _calibration[servo_id][1].setpoint_pwm)
  {
    return NAN;
  }

  float gain = (_calibration[servo_id][1].angle_deg - _calibration[servo_id][0].angle_deg) / (_calibration[servo_id][1].setpoint_pwm - _calibration[servo_id][0].setpoint_pwm);
  float offset = _calibration[servo_id][0].angle_deg;

  float value = (current_sp - _calibration[servo_id][0].setpoint_pwm) * gain + offset;

  return value;
}

void servo_set_angle(unsigned int servo_id, float angle)
{
  float current_sp = _servos[servo_id].setpoint_pwm;

  if(isnan(angle))
  {
    _servos[servo_id].setpoint_pwm = 0;
    platform_servo_set_value(servo_id, 0);
    return;
  }

  if(_calibration[servo_id][0].angle_deg == _calibration[servo_id][1].angle_deg)
  {
    _servos[servo_id].setpoint_pwm = 0;
    platform_servo_set_value(servo_id, 0);
    return;
  }

  float gain = (_calibration[servo_id][1].setpoint_pwm - _calibration[servo_id][0].setpoint_pwm) / (_calibration[servo_id][1].angle_deg - _calibration[servo_id][0].angle_deg);
  float offset = _calibration[servo_id][0].setpoint_pwm;

  float value = (current_sp - _calibration[servo_id][0].angle_deg) * gain + offset;

  _servos[servo_id].setpoint_pwm = value;
  platform_servo_set_value(servo_id, value);
}

void servo_set_pwm(unsigned int servo_id, uint16_t pwm)
{
  _servos[servo_id].setpoint_pwm = pwm;
  platform_servo_set_value(servo_id, pwm);
}
