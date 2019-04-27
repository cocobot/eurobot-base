#include <platform.h>
#include "motor_control.h"

/**
 * @brief Frequency of pwm outputs in kilohertz
 */
#define MOTOR_CONTROL_PWM_FREQUENCY_kHz 20000
#define MOTOR_CONTROL_MAX_PWM 1000
#define MOTOR_SERVO_US 100
#define MOTOR_POLES 6

/*********************************
 * Global variables definition
 *********************************/

/*************************************
 * HALL PHASE / PWM Sync - ClockWise
* Normal HALL 	#		Phase sequence
 *	#		U	V	W				DRIVE	U		V		W
 * 	0		0	0	1		1					0		PWM	HZ
 * 	1		0	1	1		3					PWM	0		HZ
 * 	2		0	1	0		2					PWM HZ	0
 * 	3		1	1	0		6					0		HZ	PWM
 * 	4		1	0	0		4					HZ	0		PWM
 * 	5		1	0	1		5					HZ	PWM	0
 *************************************/
static const unsigned int _Hall[7] = {-1, 0, 2, 1, 4, 5, 3};

/*timestamp var*/
static uint64_t _Hall_timestamp_us = 0;
static uint64_t _Servo_timestamp_us = 0;

/*motor parameter*/
static float _Velocity = 0;
static int _Phase = 0;
static volatile int64_t _Cur_PWM = 0; 

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
static void _set_motor_pwm(void); //control mosfet drivers
static int motor_control_get_phase(void); //reads HALL inputs
/*compute current speed and phase configuration*/
static void motor_control_speed(int phase, uint64_t timestamp_us); 


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
float get_velocity(void){
	return _Velocity;
}

void motor_control_init(void)
{
#if 0
	//set hall sensor input pins as interrupt source
	platform_gpio_set_interrupt(PLATFORM_GPIO_UHALL, MCUAL_GPIO_BOTH_EDGE, motor_control_hall_callback);
	platform_gpio_set_interrupt(PLATFORM_GPIO_VHALL, MCUAL_GPIO_BOTH_EDGE, motor_control_hall_callback);
	platform_gpio_set_interrupt(PLATFORM_GPIO_WHALL, MCUAL_GPIO_BOTH_EDGE, motor_control_hall_callback);
#endif

	/*dissable all PWM Drivers*/
	platform_gpio_clear(PLATFORM_GPIO_UEN | PLATFORM_GPIO_VEN | PLATFORM_GPIO_WEN);

	//init frequency and duty cycle
	platform_set_frequency(PLATFORM_PWM_U | PLATFORM_PWM_V | PLATFORM_PWM_W, MOTOR_CONTROL_PWM_FREQUENCY_kHz);
	platform_set_duty_cycle(PLATFORM_PWM_U, 0);
	platform_set_duty_cycle(PLATFORM_PWM_V, 0);
	platform_set_duty_cycle(PLATFORM_PWM_W, 0);
}


void motor_control_process_event(uint64_t timestamp_us){
	int phase;

	/*time to reevaluate servo loop*/
	if (timestamp_us - _Servo_timestamp_us > MOTOR_SERVO_US){
		// motor_compute_servo();
	}

	/*someting changed on halls*/
	if (_Phase != (phase = motor_control_get_phase())){ //hall phase has changed
		motor_control_speed(phase, timestamp_us);
	}

	_set_motor_pwm();
	return;
}

void motor_control_set_config(float kp, float ki, float imax, float max_speed_rpm)
{
	(void)kp;
	(void)ki;
	(void)imax;
	(void)max_speed_rpm;
}

void motor_control_set_setpoint(uint8_t enable, float rpm)
{
	(void)enable;
	(void)rpm;
}

/**
 * @brief Stop motor and reset speed taget
 * */
void motor_control_stop(void){
	int i;

	for (i = 0; i < 3; i++){
		platform_gpio_clear(_Driver_pins->en);
		platform_set_duty_cycle(_Driver_pins->pwm,0);
	}

	_Cur_PWM = 0;
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
static void _set_motor_pwm(void){
	int i;
	uint32_t pwm;
	int signum;
	struct motor_driver_pin * pin;
	static const int Motor_phase[6][3] = {
		{ 0, 1,-1},
		{ 1, 0,-1},
		{ 1,-1, 0},
		{ 0,-1, 1},
		{-1, 0, 1},
		{-1, 1, 0}
	};


	/*direction of rotation and PWM cast*/
	if(_Cur_PWM < 0){
		pwm = (uint32_t)( -_Cur_PWM);
		signum = -1;
	}
	else{
		pwm = (uint32_t)_Cur_PWM;
		signum = 1;
	}

	/*set IO according to direction and current phase*/
	for (i = 0; i < 3; i++){ //sweeping U, V, W phase
		pin = _Driver_pins + i; //current phase pin pointers

		switch(Motor_phase[_Phase][i] * signum){
			case -1 : // Should be HZ
				platform_gpio_clear(pin->en);
				platform_set_duty_cycle(pin->pwm, 0);
				break;

			case 0: // Should be grounded
				platform_gpio_set(pin->en);
				platform_set_duty_cycle(pin->pwm, 0);
				break;

			case 1 : // Should be PWM
				platform_gpio_set(pin->en);
				platform_set_duty_cycle(pin->pwm, pwm);
				break;

			default: //Should not append
				/*
				 * @TODO : implement error catch
				 */ 
				break;
		}
	}
	return;
}

static int motor_control_get_phase(void){

	/*get current hall value*/
	const unsigned int hall = 
		(!!platform_gpio_get(PLATFORM_GPIO_UHALL)<<2) |
		(!!platform_gpio_get(PLATFORM_GPIO_VHALL)<<1) |
		!!platform_gpio_get(PLATFORM_GPIO_VHALL);

	/* get current phase number - check for errors*/
	if ((hall >= 1) && (hall <= 5)){
		return _Hall[hall];
	}
	else{
		printerror("Invalid hall value !\n");
		return -1;
	}
}

static void motor_control_speed(int phase, uint64_t timestamp_us){
	/*check for signum and overspeed*/
	int delta_ph = phase - _Phase;
	int signum;

	if (delta_ph == 0){ //artefact
		return;
	}

	/* normal move and fringe cases*/
	if ((delta_ph == 1) || ((phase == 0) && (_Phase == 5))){ 
		/* clockwise*/
		signum = 1;
	}
	else if ((delta_ph == -1) || ((phase == 5) && (_Phase == 0))) {
		/* counter clockwise */
		signum = -1;
	}
	else { //error
		printerror("Overspeed !\n");
		return;
	}
	_Velocity = signum * MOTOR_POLES / 6.0 / (float)(_Hall_timestamp_us - timestamp_us);	

	/*uptage global variables*/

	_Phase = phase;
	_Hall_timestamp_us = timestamp_us;

	return; 
}


