#ifndef COCOBOT_CONFIG_H
#define COCOBOT_CONFIG_H

#define COCOBOT_COM_ID  COCOBOT_SMECANELE_ID

#define SERVO_NUMBER  13

#define SERVO_CALIBRATION \
{\
  [0] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 155,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 360,\
    }\
  },\
  [1] = \
  {\
    {\
      .angle_deg = -100,\
      .setpoint_pwm = 200,\
    },\
    {\
      .angle_deg = -150,\
      .setpoint_pwm = 330,\
    }\
  },\
  [2] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 180,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 390,\
    }\
  },\
  [3] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 175,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 385,\
    }\
  },\
  [4] = \
  {\
    {\
      .angle_deg = -100,\
      .setpoint_pwm = 180,\
    },\
    {\
      .angle_deg = -150,\
      .setpoint_pwm = 310,\
    }\
  },\
  [5] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 250,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 460,\
    }\
  },\
}

#endif//COCOBOT_CONFIG_H
