
//Remove definitions from make menuconfig
#undef CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG
#undef CONFIG_LIBCOCOBOT_POSITION_TICK_PER_METER
#undef CONFIG_LIBCOCOBOT_DIST_RAMP_MAX_SPEED
#undef CONFIG_LIBCOCOBOT_DIST_RAMP_MAX_ACCEL
#undef CONFIG_LIBCOCOBOT_ANGU_RAMP_MAX_SPEED
#undef CONFIG_LIBCOCOBOT_ANGU_RAMP_MAX_ACCEL
#undef CONFIG_LIBCOCOBOT_DIST_PID_KP 
#undef CONFIG_LIBCOCOBOT_DIST_PID_KD 
#undef CONFIG_LIBCOCOBOT_DIST_PID_KI 
#undef CONFIG_LIBCOCOBOT_DIST_PID_MAX_INTEGRAL 
#undef CONFIG_LIBCOCOBOT_DIST_PID_MAX_ERROR_FOR_INTEGRATION 
#undef CONFIG_LIBCOCOBOT_ANGU_PID_KP 
#undef CONFIG_LIBCOCOBOT_ANGU_PID_KD 
#undef CONFIG_LIBCOCOBOT_ANGU_PID_KI 
#undef CONFIG_LIBCOCOBOT_ANGU_PID_MAX_INTEGRAL 
#undef CONFIG_LIBCOCOBOT_ANGU_PID_MAX_ERROR_FOR_INTEGRATION 

#define COCOBOT_INVERT_RIGHT_MOTOR

//Add custom values
#define CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG  21821
#define CONFIG_LIBCOCOBOT_POSITION_TICK_PER_METER   60516

#define CONFIG_LIBCOCOBOT_DIST_RAMP_MAX_SPEED       6000
#define CONFIG_LIBCOCOBOT_DIST_RAMP_MAX_ACCEL       100
#define CONFIG_LIBCOCOBOT_ANGU_RAMP_MAX_SPEED       2000
#define CONFIG_LIBCOCOBOT_ANGU_RAMP_MAX_ACCEL       300

#define CONFIG_LIBCOCOBOT_DIST_PID_KP               2000
#define CONFIG_LIBCOCOBOT_DIST_PID_KD               4000
#define CONFIG_LIBCOCOBOT_DIST_PID_KI               100
#define CONFIG_LIBCOCOBOT_DIST_PID_MAX_INTEGRAL     1000
#define CONFIG_LIBCOCOBOT_DIST_PID_MAX_ERROR_FOR_INTEGRATION  50 

#define CONFIG_LIBCOCOBOT_ANGU_PID_KP               4000 
#define CONFIG_LIBCOCOBOT_ANGU_PID_KD               20000
#define CONFIG_LIBCOCOBOT_ANGU_PID_KI               100
#define CONFIG_LIBCOCOBOT_ANGU_PID_MAX_INTEGRAL     1000
#define CONFIG_LIBCOCOBOT_ANGU_PID_MAX_ERROR_FOR_INTEGRATION  30 
