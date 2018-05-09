#ifndef COCOBOT_ASSERV_H
#define COCOBOT_ASSERV_H

typedef enum
{
  COCOBOT_ASSERV_DISABLE,
  COCOBOT_ASSERV_ENABLE,
} cocobot_asserv_state_t;

/* Initialization of the asserv module. Need to be called before any other action 
 */
void cocobot_asserv_init(void);

/* Run the asserv computation
 */
void cocobot_asserv_compute(void);

/* Set the curvilinear distance set point
 * Argument:
 *  - distance: requested set point (mm)
 */
void cocobot_asserv_set_distance_set_point(float distance);

/* Set the angular set point
 * Argument:
 *  - distance: requested set point (deg)
 */
void cocobot_asserv_set_angular_set_point(float angular);

/* Enable/disable the asserv
 * Argument:
 *  - state: requested cocobot_asserv_state_t value (COCOBOT_ASSERV_DISABLE or COCOBOT_ASSERV_ENABLE)
 */
void cocobot_asserv_set_state(cocobot_asserv_state_t state);

/* Get the asserv of the asserv
 * Return:
 *  - cocobot_asserv_state_t value (COCOBOT_ASSERV_DISABLE or COCOBOT_ASSERV_ENABLE)
 */
cocobot_asserv_state_t cocobot_asserv_get_state(void);

/* Handle console user command related to asserv module
 * Argument:
 *  - pid: requested command identifier
 */
void cocobot_asserv_com_handler(uint16_t pid);

/* Send asynchronously debug informations
 */
void cocobot_asserv_handle_async_com(void);

/* Get the linear speed of the robot (assumpe acceleration is inifite)
 * Return:
 *  - the linear speed in m/s
 */
float cocobot_asserv_get_linear_speed(void);

void cocobot_asserv_slow(void);

void cocobot_asserv_handle_sync_com(uint16_t pid, uint8_t * data, uint32_t len);

#endif// COCOBOT_ASSERV_H
