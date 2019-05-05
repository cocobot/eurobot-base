#include <platform.h>
#include <cocobot.h>
#include "motor_control.h"
#include "pid.h"
#include <stdio.h>
#include <stdarg.h>


/**
 * @brief Frequency of pwm outputs in kilohertz
 */
#define MOTOR_CONTROL_PWM_FREQUENCY_kHz 20000
#define MOTOR_CONTROL_MAX_PWM 1000000
#define MOTOR_CONTROL_PWM_FACTOR 0.1
#define MOTOR_CONTROL_SERVO_REFRES_US 20000
#define MOTOR_CONTROL_POLES 8
#define MOTOR_CONTROL_GEAR 5

#define MOTOR_CONTROL_HALL_ISR 1

/*debug*/
#define MOTOR_CONTROL_DEBUG_EN 1
#define MOTOR_CONTROL_DEBUG_PRINT 500000
#define MOTOR_CONTROL_DEBUG_BUFFER 255
#define MOTOR_CONTROL_DEBUG_PRINT_PHASE 1
#define MOTOR_CONTROL_DEBUG_PRINT_HALL_VALUE 0
#define MOTOR_CONTROL_DEBUG_PRINT_VELOCITY 1
#define MOTOR_CONTROL_WARN_LAG 0 //direct print

/*********************************
 * Global variables definition
 *********************************/
/*motor parameter*/
static float _Velocity = 0;
static volatile int _Phase = 0;
static volatile int _Delta_phase ;
static volatile int32_t _Pwm = 0;

struct motor_driver_pin {
	uint32_t en;
	uint32_t pwm;
};

static struct motor_driver_pin _Driver_pins[3]={
	{PLATFORM_GPIO_UEN, PLATFORM_PWM_U},
	{PLATFORM_GPIO_VEN, PLATFORM_PWM_V},
	{PLATFORM_GPIO_WEN, PLATFORM_PWM_W}
};

/********************************
 * Private functions declaration
 ********************************/
static int _motor_control_get_hall(void); //reads HALL inputs
/*compute current speed and phase configuration*/
static float _motor_control_update_speed( uint64_t timestamp_us); 

#if MOTOR_CONTROL_HALL_ISR
static void _motor_control_update_callback(void);
#else
static void _set_motor_pwm(int32_t pwm, int phase); //control mosfet drivers
#endif 

#if MOTOR_CONTROL_DEBUG_EN
/* for debug only*/
static char _Debug_buffer[MOTOR_CONTROL_DEBUG_BUFFER];
static unsigned int _Dbg_idx = 0;


#if 1
static void print(char const * format, ...){
	int count;
	va_list args;
	int left = MOTOR_CONTROL_DEBUG_BUFFER - _Dbg_idx - 1;

	if (left > 2){
		va_start(args, format);
		count = vsnprintf(_Debug_buffer + _Dbg_idx, (size_t)left, format, args);
		va_end(args);
		if (count > 0){
			_Dbg_idx += count; 
		}
	}
	return;
}

#endif
static void print_uart(void){
	if (_Dbg_idx > 0){
		uprintf("%s",_Debug_buffer);
		_Dbg_idx = 0;
	}
	return;
}

#else

static void print(char const * str,...){
	(void)str;
	return;
}
#endif

/*******************************
 * Public Function definition
 *******************************/
/**
 * @brief Return current velocity, based on Hall sensors
 */
float motor_control_get_velocity(void){
	return _Velocity;
}

void motor_control_init(void){
#if MOTOR_CONTROL_HALL_ISR
 platform_gpio_set_interrupt(
		 PLATFORM_GPIO_UHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);
 platform_gpio_set_interrupt(
		 PLATFORM_GPIO_VHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);
 platform_gpio_set_interrupt(
		 PLATFORM_GPIO_WHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);
#endif

	/*dissable all PWM Drivers*/
	platform_gpio_clear( 
			PLATFORM_GPIO_UEN | 
			PLATFORM_GPIO_VEN | 
			PLATFORM_GPIO_WEN);

	//init frequency and duty cycle
	platform_set_frequency(
			PLATFORM_PWM_U | 
			PLATFORM_PWM_V | 
			PLATFORM_PWM_W, 
			MOTOR_CONTROL_PWM_FREQUENCY_kHz);
	platform_set_duty_cycle(PLATFORM_PWM_U, 0);
	platform_set_duty_cycle(PLATFORM_PWM_V, 0);
	platform_set_duty_cycle(PLATFORM_PWM_W, 0);
	uprintf("INIT DONE !\n");
}


void motor_control_process_event(uint64_t timestamp_us){
	static uint64_t servo_timestamp_us = 0;
	uint64_t dt;
	static int flag = 1;
	static float speed_val =50000.0;
	float velocity = _motor_control_update_speed(timestamp_us);
	//static float speed_val = 80000.0;

	/*time to reevaluate servo loop*/
	dt = timestamp_us - servo_timestamp_us;

	if(dt > 100000 ){
		print("cons %d speed %d\n",(long int)speed_val,(int)(velocity*1000));

		if ( speed_val > 500000.0){
			flag = -1;
		}if (speed_val < 20000.0){
			flag = 1;
		}

		speed_val += flag *1000;

		_Pwm =  ((int32_t)(speed_val * MOTOR_CONTROL_PWM_FACTOR));
			servo_timestamp_us = timestamp_us;
			_motor_control_update_callback();
	}


	if (dt > MOTOR_CONTROL_SERVO_REFRES_US){

#if MOTOR_CONTROL_WARN_LAG
		if (dt / MOTOR_CONTROL_SERVO_REFRES_US >= 2){
			uprintf("LAG ! : %d", dt / MOTOR_CONTROL_SERVO_REFRES_US);
		}
#endif
#if 0
		/*compute pid*/
		speed_val = pid_update(_Velocity, dt);
		if (pid_is_limited()){
		//	print("Warning : Quadramp or speed limit\n");
		}

		/*transform for float to pwm (signed) and update pwm*/
		_set_motor_pwm((int32_t)(speed_val * MOTOR_CONTROL_PWM_FACTOR));
		servo_timestamp_us = timestamp_us;
		uprintf("%d\n",(int32_t)(speed_val * MOTOR_CONTROL_PWM_FACTOR));		
#endif
	}


#if MOTOR_CONTROL_DEBUG_EN
	static uint64_t dbg_timestamp_us = 0;
	/* time to print*/
	if (timestamp_us - dbg_timestamp_us > MOTOR_CONTROL_DEBUG_PRINT){
		print_uart();
		dbg_timestamp_us = timestamp_us;
	}
#endif

	return;
}

void motor_control_set_config(float kp, float ki, float imax, float max_speed_rpm)
{
	pid_set(kp, ki, 0.0);
	pid_set_limit(max_speed_rpm,imax);
	pid_set_cons(0.0);
	pid_reset();

	return;
}

void motor_control_set_setpoint(uint8_t enable, float rpm)
{
	int i;

	if (enable){
		pid_set_cons(rpm);
	}
	else {
		for (i = 0; i < 3; i++){
			platform_gpio_clear(_Driver_pins->en);
			platform_set_duty_cycle(_Driver_pins->pwm,0);
		}

		pid_set_cons(0.0);
		pid_reset();
	}

	return;
}


/**********************************
 * Private Function Definition
 ***********************************/
/*
 * @brief Private function. Set PWM/EN to each drivers
 * See _Hall comment for drive definitions
 * Note : Not optimized for execution time AT ALL
 * Should be ok though
 */
/************************************************
 * HALL PHASE 						 PWM Sync - ClockWise
 * Phase HALL 		Val Val-1			Phase sequence
 *	#		U	V	W							DRIVE	U		V		W
 * 	0		0	0	1		1		0						0		PWM	HZ
 * 	1		0	1	1		3		2						PWM	0		HZ
 * 	2		0	1	0		2		1						PWM HZ	0
 * 	3		1	1	0		6		5						0		HZ	PWM
 * 	4		1	0	0		4		3						HZ	0		PWM
 * 	5		1	0	1		5		4						HZ	PWM	0
 ***********************************************/
/**
 * @brief : convert hall sensors value to phase value
 * @return phase number, -1 if error
 * */
static int _motor_control_get_hall(void){
	static const unsigned int hall_to_phase[6] = {0, 2, 1, 4, 5, 3};

	const unsigned int hall = //get current hall values
		(!!platform_gpio_get(PLATFORM_GPIO_UHALL)<<2) |
		(!!platform_gpio_get(PLATFORM_GPIO_VHALL)<<1) |
		!!platform_gpio_get(PLATFORM_GPIO_WHALL);

	if ((hall >= 1) && (hall <= 6)){ //hall value ok
		return hall_to_phase[hall - 1];
	}
	else{ //not a valid value
		return -1;
	}
}

#if MOTOR_CONTROL_HALL_ISR
static void _motor_control_update_callback(void){
	int i;
	struct motor_driver_pin * pin;
	int32_t motor_pin_val;
	static const int32_t motor_phases[6][3] = {
		{ 1, 0,-1},
		{-1, 0, 1},
		{ 0,-1, 1},
		{ 0, 1,-1},
		{-1, 1, 0},
		{ 1,-1, 0}
	};
	int phase = _motor_control_get_hall();

	if (phase == -1){ //something wrong
		return;
	}
	
	_Delta_phase += 1;

	if (_Pwm > 0){
		phase = (phase + 1) % 6;
	}
	else if (_Pwm < 0){
		phase = (phase - 1) % 6;
	}
	else {
		motor_control_set_setpoint(0,0.0);
		return;
	}

	/*set IO according to direction and current phase*/
	for (i = 0; i < 3; i++){ //sweeping U, V, W phase
		
		pin = &(_Driver_pins[i]); //current phase pin pointers
		motor_pin_val = _Pwm * motor_phases[phase][i]; //current pwm value 
		
		if (motor_pin_val > 0){ //should be PWM
			platform_set_duty_cycle(pin->pwm, _Pwm);
			platform_gpio_set(pin->en);
		}
		else if (motor_pin_val == 0){ // Should be grounded
			platform_set_duty_cycle(pin->pwm, 0);
			platform_gpio_set(pin->en);
		} 
		else { // should be HZ
			platform_gpio_clear(pin->en);
			platform_set_duty_cycle(pin->pwm, 0);
		}
	}
	
	return;
}
#else
static void _set_motor_pwm(int32_t pwm, int phase){
	int i;
	struct motor_driver_pin * pin;
	int32_t motor_pin_val;
/*
	static const int32_t motor_phases[6][3] = {
		{ 1, 0,-1},
		{-1, 1, 0},
		{ 0,-1, 1},
		{ 1, 0,-1},
		{-1, 1, 0},
		{ 0,-1, 1}
	};
*/
	static const int32_t motor_phases[6][3] = {
		{ 1, 0,-1},
		{-1, 0, 1},
		{ 0,-1, 1},
		{ 0, 1,-1},
		{-1, 1, 0},
		{ 1,-1, 0}
	};

	/*set IO according to direction and current phase*/
	for (i = 0; i < 3; i++){ //sweeping U, V, W phase
		pin = &(_Driver_pins[i]); //current phase pin pointers
		motor_pin_val = pwm * motor_phases[phase][i]; //current pwm value 
		if (motor_pin_val > 0){ //should be PWM
			platform_set_duty_cycle(pin->pwm, pwm);
			platform_gpio_set(pin->en);
		}
		else if (motor_pin_val == 0){ // Should be grounded
			platform_set_duty_cycle(pin->pwm, 0);
			platform_gpio_set(pin->en);
		} 
		else { // should be HZ
			platform_gpio_clear(pin->en);
			platform_set_duty_cycle(pin->pwm, 0);
		}
	}
	return;
}


#endif


/**
 * @brief : compute motor angular speed according to hall sensors
 * @return : -1 if overspeed (dt = 0 or hall delta >=3)
 * @return : 0 if nothing changed
 * @return : 1 if update
 */
static float _motor_control_update_speed(uint64_t timestamp_us){
	
	static uint64_t hall_timestamp_us = 0;
	float dangle; //angle delta in fraction of turn
	float velocity;
	uint64_t dt; //get time delta;
	dt = timestamp_us - hall_timestamp_us; //get time delta;

	/*update global variables*/
	uprintf("dp %d",_Delta_phase);

	dangle = ( _Delta_phase) / (MOTOR_CONTROL_POLES * 12); 
	velocity = (dangle * 1000000) / dt  * 60;
	_Phase = (_Phase + _Delta_phase) % 6;
  _Delta_phase = 0;
	hall_timestamp_us = timestamp_us;

	return velocity; 
}


