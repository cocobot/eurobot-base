#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	size_t
#define portBASE_TYPE	long
#define portPOINTER_SIZE_TYPE size_t

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	#define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/

#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portINLINE __inline

#if defined( __x86_64__) || defined( _M_X64 )
	#define portBYTE_ALIGNMENT		8
#else
	#define portBYTE_ALIGNMENT		4
#endif



///* Critical section management. */
//
//#define portDISABLE_INTERRUPTS()	asm volatile ( "cli" :: );
//#define portENABLE_INTERRUPTS()		asm volatile ( "sei" :: );
///*-----------------------------------------------------------*/
//
///* Architecture specifics. */
//#define portBYTE_ALIGNMENT			1
//#define portNOP()					asm volatile ( "nop" );
///*-----------------------------------------------------------*/

/* Kernel utilities. */
#define portINTERRUPT_YIELD				( 0UL )
#define portINTERRUPT_TICK				( 1UL )
#define portYIELD()					vPortGenerateSimulatedInterrupt( portINTERRUPT_YIELD )
/*-----------------------------------------------------------*/

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portDISABLE_INTERRUPTS() vPortEnterCritical()
#define portENABLE_INTERRUPTS() vPortExitCritical()
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()

void vPortEnterCritical( void );
void vPortExitCritical( void );
void vPortGenerateSimulatedInterrupt( uint32_t ulInterruptNumber );

#endif /* PORTMACRO_H */

