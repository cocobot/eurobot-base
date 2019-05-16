#include <platform.h>
#include <cocobot.h>
#include "motor_control.h"
#include <stdio.h>
#include <stdarg.h>
#include <stm32l4xx.h>

/**
 * @brief Frequency of pwm outputs in kilohertz
 */
// 0.05 rpm/PWM
#define MOTOR_CONTROL_PWM_FREQUENCY_kHz 20000
#define MOTOR_CONTROL_MAX_PWM 1000000
#define MOTOR_CONTROL_PWM_FACTOR 200.0
#define MOTOR_CONTROL_SERVO_REFRES_US 10000
#define MOTOR_CONTROL_POLES 8
#define MOTOR_CONTROL_GEAR 5


/*debug*/
#define MOTOR_CONTROL_DEBUG_EN 1
#define MOTOR_CONTROL_DEBUG_PRINT 500000
#define MOTOR_CONTROL_DEBUG_BUFFER 555
#define MOTOR_CONTROL_DEBUG_PRINT_PHASE 1
#define MOTOR_CONTROL_DEBUG_PRINT_HALL_VALUE 0
#define MOTOR_CONTROL_DEBUG_PRINT_VELOCITY 1
#define MOTOR_CONTROL_WARN_LAG 0 //direct print
#define MOTOR_CONTROL_DEMO 1

/*********************************
 * Global variables definition
 *********************************/
/*motor parameter*/
static volatile int _Phase = 0;
static volatile int _Delta_phase ;
static volatile int32_t _Pwm = 0;
static float _max_acc = 1000000.0;
static float _max_speed = 2000000.0;
static volatile float _speed_cons = 0.0;
static volatile uint64_t _time;

/*flags*/
static volatile int _quad_limit = 0;
static volatile int _speed_limit = 0;


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
static inline int _motor_control_get_hall(void); //reads HALL inputs
/*compute current speed and phase configuration*/
static void _motor_control_update_callback(void);
static inline float _quadramp(float target_speed, uint64_t dt);
static inline float _limit_out(float speed);

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

void motor_control_init(void){
	platform_gpio_set_interrupt(
			PLATFORM_GPIO_UHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);
	platform_gpio_set_interrupt(
			PLATFORM_GPIO_VHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);
	platform_gpio_set_interrupt(
			PLATFORM_GPIO_WHALL, MCUAL_GPIO_BOTH_EDGE,_motor_control_update_callback);

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
	uint64_t dt = timestamp_us - servo_timestamp_us;

	_time = timestamp_us;

#if MOTOR_CONTROL_DEMO
	static float speed_val = -60.0;
	static int stamp = 100;

	if(dt > 1000 ){
		if (!(stamp--)){
			_speed_cons =  speed_val;
			speed_val *= -1;
			stamp = 5000;
		}
		_Pwm = (int32_t)(_limit_out(_quadramp(_speed_cons,dt)) * MOTOR_CONTROL_PWM_FACTOR);
		_motor_control_update_callback();
	}

#else
	if (dt > MOTOR_CONTROL_SERVO_REFRES_US){
		/*quadramp calculation*/
		_Pwm = (int32_t)(_limit_out(_quadramp(_speed_cons,dt)) * MOTOR_CONTROL_PWM_FACTOR);
		_motor_control_update_callback();
		servo_timestamp_us = timestamp_us;
		print("%d %d %d\n",_quad_limit, _speed_limit,_Pwm);
	}
#endif

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

void motor_control_set_config(float imax, float max_speed_rpm){
	_max_speed = max_speed_rpm;
	_max_acc = imax;
	return;
}

void motor_control_set_setpoint(uint8_t enable, float rpm){
	int i;

	if (enable){
		_speed_cons = rpm;
	}
	else {
		_speed_cons = 0.0;
		for (i = 0; i < 3; i++){
			platform_gpio_clear(_Driver_pins->en);
			platform_set_duty_cycle(_Driver_pins->pwm,0);
		}

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
static inline int _motor_control_get_hall(void){
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

static void _motor_control_update_callback(void){
	__disable_irq();
	int i;
	int32_t pwm;
	struct motor_driver_pin * pin;
	int32_t motor_pin_val;
	int phase; 
	static int old_phase = 0;
	int dphase;
	static const int32_t motor_phases[6][3] = {
		{ 1, 0,-1},
		{-1, 0, 1},
		{ 0,-1, 1},
		{ 0, 1,-1},
		{-1, 1, 0},	
		{ 1,-1, 0}
	};

	static uint64_t oldtime = 0;

	phase = _motor_control_get_hall();

	if (phase == -1){ //something wrong
		__enable_irq();
		return;
	}

	dphase = phase - old_phase;
	old_phase = phase;

	if (dphase <= -3){
		_Delta_phase = dphase + 6;
	}
	else if (dphase >= 3){
		_Delta_phase = dphase - 6;
	}
	else{
		_Delta_phase = dphase;
	}

	if ((_Delta_phase == -1) || (_Delta_phase == 1)){
		print("%d,%d\n",_Pwm,(int)(_time-oldtime) );
		oldtime = _time;
	}
	if (_Pwm > 0){
		phase = (phase + 1) % 6;
		pwm = _Pwm;
	}
	else if (_Pwm < 0){
		phase += (phase > 1) ? -2 : 4;
		pwm = - _Pwm;
	}
	else {
		motor_control_set_setpoint(0,0.0);
		__enable_irq();
		return;
	}

	/*set IO according to direction and current phase*/
	for (i = 0; i < 3; i++){ //sweeping U, V, W phase

		pin = &(_Driver_pins[i]); //current phase pin pointers
		motor_pin_val = motor_phases[phase][i]; //current pwm value 

		if (motor_pin_val > 0){ //should be PWM
			platform_set_duty_cycle(pin->pwm, pwm );
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

	__enable_irq();
	return;
}

static inline float _quadramp(float target_speed, uint64_t dt){
	static float current_speed = 0.0;
	float acc = (target_speed - current_speed) / (float)dt;	

	print("quad %d",(int)acc);

	if (dt == 0){ //just to be shure
		return current_speed;
	}

	if ((acc > -_max_acc) && (acc < _max_acc)){//acceleration below limmit
		_quad_limit = 0;
		current_speed = target_speed;
		return target_speed;
	}
	else{ //quadramp limit
		_quad_limit = 1;
		if (acc > 0){ //increse speed
			current_speed += _max_acc * (float)dt;
		}
		else{ //decrease speed
			current_speed -= _max_acc * (float)dt;
		}
		return current_speed;
	}
	return 0.0; //never reached
}


static inline float _limit_out(float speed){

	if ((speed > -_max_speed) && (speed < _max_speed)){// in normal range
		_speed_limit = 0;
		return speed;
	}
	else {
		_speed_limit = 1;
		if (speed < 0.0){
			return -_max_speed;
		}
		else {
			return _max_speed;
		}
	}

	return 0.0; //never reached
}
