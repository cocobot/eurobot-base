#ifndef COCOBOT_CONFIG_H
#define COCOBOT_CONFIG_H

#define SERVO_NUMBER  3

#define SERVO_CALIBRATION \
{\
  [0] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 200,\
    },\
    {\
      .angle_deg = 180,\
      .setpoint_pwm = 500,\
    }\
  },\
  [1] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 200,\
    },\
    {\
      .angle_deg = 180,\
      .setpoint_pwm = 500,\
    }\
  },\
  [2] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 200,\
    },\
    {\
      .angle_deg = 180,\
      .setpoint_pwm = 500,\
    }\
  },\
}

#endif//COCOBOT_CONFIG_H
