#ifndef COCOBOT_CONFIG_H
#define COCOBOT_CONFIG_H

#define SERVO_NUMBER  12

#define SERVO_CALIBRATION \
{\
  [0] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 210,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 420,\
    }\
  },\
  [1] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 150,\
    },\
    {\
      .angle_deg = -90,\
      .setpoint_pwm = 370,\
    }\
  },\
  [2] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 375,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 200,\
    }\
  },\
  [3] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 200,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 410,\
    }\
  },\
  [4] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 190,\
    },\
    {\
      .angle_deg = -90,\
      .setpoint_pwm = 405,\
    }\
  },\
  [5] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 375,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 160,\
    }\
  },\
  [6] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 185,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 400,\
    }\
  },\
  [7] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 185,\
    },\
    {\
      .angle_deg = -90,\
      .setpoint_pwm = 400,\
    }\
  },\
  [8] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 325,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 140,\
    }\
  },\
  [9] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 175,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 405,\
    }\
  },\
  [10] = \
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 190,\
    },\
    {\
      .angle_deg = -90,\
      .setpoint_pwm = 415,\
    }\
  },\
  [11] =\
  {\
    {\
      .angle_deg = 0,\
      .setpoint_pwm = 335,\
    },\
    {\
      .angle_deg = 90,\
      .setpoint_pwm = 150,\
    }\
  },\
}

#endif//COCOBOT_CONFIG_H
