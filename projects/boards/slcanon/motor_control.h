#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

float motor_control_get_velocity(void);

/**
 * @brief Intialisation of the motor control algorith
 */
void motor_control_init(void);


/**
 * @brief Run the algorithm. This function is called as fast as possible on the main loop
 * @param timestamp_us  Number of microseconds since the begining of the program
 */
void motor_control_process_event(uint64_t timestamp_us);


void motor_control_set_config(float imax, float max_speed_rpm);
void motor_control_get_config(float * imax, float * max_speed_rpm);

void motor_control_set_setpoint(uint8_t enable, float rpm);

uint8_t motor_control_is_enabled(void);
float motor_control_get_setpoint(void);
int32_t motor_control_get_pwm(void);

#endif // MOTOR_CONTROL_H
