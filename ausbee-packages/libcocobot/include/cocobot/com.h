#ifndef COCOBOT_COM_H
#define COCOBOT_COM_H

#include <mcual.h>
#include <string.h>

//Format specification
// - H : u16
// - F : 32bit float
// - B : byte

#define COCOBOT_COM_POSITION_DEBUG_PID    (0x8000)
#define COCOBOT_COM_ASSERV_DIST_DEBUG_PID (0x8001)
#define COCOBOT_COM_ASSERV_ANGU_DEBUG_PID (0x8002)
#define COCOBOT_COM_TRAJECTORY_DEBUG_PID  (0x8003)
#define COCOBOT_COM_PATHFINDER_DEBUG_PID  (0x8004)
#define COCOBOT_COM_PRINTF_PID            (0x8005)
#define COCOBOT_COM_GAME_STATE_DEBUG_PID  (0x8006)
#define COCOBOT_COM_RESET_PID             (0x8007)
#define COCOBOT_COM_ACTION_SCHEDULER_DEBUG_PID (0x8008)
#define COCOBOT_COM_GET_USIR_PID          (0x8009)
#define COCOBOT_COM_SEND_USIR_PID         (0x800A)
#define COCOBOT_COM_FORCE_USIR_PID        (0x800B)

typedef void (*cocobot_com_handler_t)(uint16_t pid);

/* Initialization of the com module. Need to be called before any other action 
 * Argument:
 *  - usart_id: usart id (from mcual) for the com
 *  - priority_monitor: priority of the reception task. This task handle user request
 *  - priority_async: priority of the task sending asynchronous debug messages (like postion, asserv...)
 *  - handler: function pointer for handling user defined commands
 */
void cocobot_com_init(mcual_usart_id_t usart_id, unsigned int priority_monitor, unsigned int priority_async, cocobot_com_handler_t handler);

/* Send packet to the user
 * Argument:
 *  - pid_id: Identifier for the packet format
 *  - fmt: define the binary format of the packet. Must be a string (cf Format specification)
 */
void cocobot_com_send(uint16_t pid, char * fmt, ...);

void cocobot_com_printf(char * fmt, ...);

uint32_t cocobot_com_read_B(uint8_t *data , uint32_t len, uint32_t offset, uint8_t * value);

#endif// COCOBOT_COM_H
