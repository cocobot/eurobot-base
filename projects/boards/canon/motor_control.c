#include <platform.h>
#include "motor_control.h"
#include "pid.h"

/**
 * @brief Frequency of pwm outputs in kilohertz
 */
#define MOTOR_CONTROL_PWM_FREQUENCY_kHz 20000
#define MOTOR_CONTROL_MAX_PWM 1000
#define MOTOR_CONTROL_PWM_FACTOR 0.1
#define MOTOR_CONTROL_SERVO_REFRES_US 100
#define MOTOR_CONTROL_POLES 6

/*********************************
 * Global variables definition
 *********************************/


/*timestamp var*/
static uint64_t _Hall_timestamp_us = 0;
static uint64_t _Servo_timestamp_us = 0;

/*motor parameter*/
static float _Velocity = 0;
static int _Phase = 0;

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
static void _set_motor_pwm(int32_t pwm); //control mosfet drivers
static int _motor_control_get_hall(void); //reads HALL inputs
/*compute current speed and phase configuration*/
static int _motor_control_update_speed(int phase, uint64_t timestamp_us); 


/* for debug only*/
void printerror(char* c){
	(void)c;
}



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
	/*dissable all PWM Drivers*/
	platform_gpio_clear(PLATFORM_GPIO_UEN | PLATFORM_GPIO_VEN | PLATFORM_GPIO_WEN);

	//init frequency and duty cycle
	platform_set_frequency(PLATFORM_PWM_U | PLATFORM_PWM_V | PLATFORM_PWM_W, MOTOR_CONTROL_PWM_FREQUENCY_kHz);
	platform_set_duty_cycle(PLATFORM_PWM_U, 0);
	platform_set_duty_cycle(PLATFORM_PWM_V, 0);
	platform_set_duty_cycle(PLATFORM_PWM_W, 0);
}


void motor_control_process_event(uint64_t timestamp_us){
	int phase = _motor_control_get_hall();
	float speed_val;

	if (phase == -1){// error on hall
		printerror("Invalid Hall value !\n");
		return;
	}
	if (_Phase != phase){ //motor positon changed. Compute new speed
		if (_motor_control_update_speed(phase, timestamp_us) < 0){
			printerror("Invalid Hall value !\n");
			return;
		}
	}

	/*time to reevaluate servo loop*/
	if (timestamp_us - _Servo_timestamp_us > MOTOR_CONTROL_SERVO_REFRES_US){

		/*compute pid*/
		speed_val = pid_update(_Velocity, timestamp_us - _Servo_timestamp_us);
		if (pid_is_limited()){
			printerror("Warning : Quadramp or speed limit\n");
		}

		/*transform for float to pwm (signed) and update pwm*/
		_set_motor_pwm((int32_t)(speed_val * MOTOR_CONTROL_PWM_FACTOR));
		_Servo_timestamp_us = timestamp_us;
	}

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
static void _set_motor_pwm(int32_t pwm){
	int i;
	struct motor_driver_pin * pin;
	int32_t motor_pin_val;
	static const int32_t motor_phases[6][3] = {
		{ 0, 1,-1},
		{ 1, 0,-1},
		{ 1,-1, 0},
		{ 0,-1, 1},
		{-1, 0, 1},
		{-1, 1, 0}
	};

	/*set IO according to direction and current phase*/
	for (i = 0; i < 3; i++){ //sweeping U, V, W phase

		pin = &(_Driver_pins[i]); //current phase pin pointers
		motor_pin_val = pwm * motor_phases[_Phase][i]; //current pwm value 

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
		!!platform_gpio_get(PLATFORM_GPIO_VHALL);

	if ((hall >= 1) && (hall <= 6)){ //hall value ok
		return hall_to_phase[hall - 1];
	}
	else{ //not a valid value
		return -1;
	}
}


/**
 * @brief : compute motor angular speed according to hall sensors
 * @return : -1 if overspeed (dt = 0 or hall delta >=3)
 * @return : 0 if nothing changed
 * @return : 1 if update
 */
static int _motor_control_update_speed(int phase, uint64_t timestamp_us){

	int delta_ph = phase - _Phase; //get phase delta.
	uint64_t dt = timestamp_us - _Hall_timestamp_us; //get time delta;
	float dangle; //angle delta in fraction of turn

	if (dt == 0){ // somethings wrong
		return -1;
	}

	/*correct the phase delta (rollover at 5)*/
	if (delta_ph <= -3){
		delta_ph += 6;
	}

	if (delta_ph == 0){ //artefact
		return 0;
	}
	if (delta_ph == 3){ //impossible to tell direction of rotation
		return -1;
	}

	/*update global variables*/

	dangle = (float)delta_ph  / (float)(MOTOR_CONTROL_POLES * 6); 
	_Velocity = dangle / ((float)(dt) * 1e-6 ) * 60;
	_Phase = phase;
	_Hall_timestamp_us = timestamp_us;

	return 1; 
}


