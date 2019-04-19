#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>


/**
 * @brief Intialisation of the motor control algorith
 */
void motor_control_init(void);


/**
 * @brief Run the algorithm. This function is called as fast as possible on the main loop
 * @param timestamp_us  Number of microseconds since the begining of the program
 */
void motor_control_process_event(uint64_t timestamp_us);


#endif // MOTOR_CONTROL_H
