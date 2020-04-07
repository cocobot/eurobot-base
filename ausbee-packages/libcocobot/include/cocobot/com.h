#ifndef COCOBOT_COM_H
#define COCOBOT_COM_H

#include <mcual.h>
#include <string.h>

//Format specification
// - H : u16
// - F : 32bit float
// - B : byte

#define COCOBOT_COM_ERROR_PID (0x0001)

#define COCOBOT_COM_SET_SERVO_PID     (0x1000)
#define COCOBOT_COM_MECA_ACTION_PID   (0x1001)

#define COCOBOT_COM_SET_MOTOR_PID     (0x2000)
#define COCOBOT_COM_SET_MOTOR_CFG_PID (0x2001)
#define COCOBOT_COM_REQ_MOTOR_CFG_PID (0x2002)
#define COCOBOT_COM_REQ_MOTOR_DBG_PID (0x2003)
#define COCOBOT_COM_MOTOR_DBG_PID     (0x2004)
#define COCOBOT_COM_SET_MOTOR_MST_PID (0x2005)

#define COCOBOT_COM_POSITION_DEBUG_PID    (0x8000)
#define COCOBOT_COM_ASSERV_DIST_DEBUG_PID (0x8001)
#define COCOBOT_COM_ASSERV_ANGU_DEBUG_PID (0x8002)
#define COCOBOT_COM_TRAJECTORY_DEBUG_PID  (0x8003)
#define COCOBOT_COM_PATHFINDER_DEBUG_PID  (0x8004)
#define COCOBOT_COM_PRINTF_PID            (0x8005)
#define COCOBOT_COM_GAME_STATE_DEBUG_PID  (0x8006)
#define COCOBOT_COM_RESET_PID             (0x8007)
#define COCOBOT_COM_ACTION_SCHEDULER_DEBUG_PID (0x8008)
//#define COCOBOT_COM_GET_USIR_PID          (0x8009)
//#define COCOBOT_COM_SEND_USIR_PID         (0x800A)
//#define COCOBOT_COM_FORCE_USIR_PID        (0x800B)
#define COCOBOT_COM_GET_ASSERV_PARAMS_PID (0x800C)
#define COCOBOT_COM_SEND_ASSERV_PARAMS_PID (0x800D)
#define COCOBOT_COM_SET_ASSERV_PARAMS_PID (0x800E)
#define COCOBOT_COM_PING_PID              (0x800F)
#define COCOBOT_COM_BOOTLOADER_PID        (0x8010)
#define COCOBOT_COM_BOOTLOADER_CRC_PID    (0x8011)
#define COCOBOT_COM_BOOTLOADER_ERASE_PID  (0x8012)
#define COCOBOT_COM_BOOTLOADER_FLASH_PID  (0x8013)
#define COCOBOT_COM_BOOTLOADER_BOOT_PID   (0x8014)


#define COCOBOT_SMECANELE_ID              0x01
#define COCOBOT_SBRAIN_ID                 0x02
#define COCOBOT_SCOM_ID                   0x03
#define COCOBOT_SLCANON_ID                0x04
#define COCOBOT_SRCANON_ID                0x05

#define COCOBOT_PMECANELE_ID              0x09
#define COCOBOT_PBRAIN_ID                 0x0A
#define COCOBOT_PCOM_ID                   0X0B
#define COCOBOT_PLCANON_ID                0X0C
#define COCOBOT_PRCANON_ID                0X0D

#define COCOBOT_COCOUI_ID                 0x11

typedef void (*cocobot_com_handler_t)(uint16_t pid, uint8_t * data, uint32_t len);

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

uint8_t cocobot_com_get_src(void);

uint32_t cocobot_com_read_B(uint8_t *data , uint32_t len, uint32_t offset, uint8_t * value);
uint32_t cocobot_com_read_D(uint8_t *data , uint32_t len, uint32_t offset, int32_t * value);
uint32_t cocobot_com_read_H(uint8_t *data , uint32_t len, uint32_t offset, int16_t * value);
uint32_t cocobot_com_read_F(uint8_t *data , uint32_t len, uint32_t offset, float * value);

#endif// COCOBOT_COM_H

