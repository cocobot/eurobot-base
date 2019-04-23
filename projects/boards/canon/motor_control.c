#include <platform.h>
#include "motor_control.h"

/**
 * @brief Frequency of pwm outputs in kilohertz
 */
#define MOTOR_CONTROL_PWM_FREQUENCY_kHz 20000
#define MOTOR_CONTROL_MAX_PWM 1000
#define MOTOR_SERVO_US 100
#define MOTOR_POLES 6


/*timestamp var*/
static uint64_t _Hall_timestamp_us = 0;
//static uint64_t _Servo_timestamp_us = 0;

/*motor parameter*/
static float _Velocity = 0;
static int _Phase = 0;
static volatile uint32_t _Cur_PWM = 0; 

/* for debug only*/
void printerror(char* c){
(void)c;
}

/* Normal HALL 		Phase sequence
 *	#		U	V	W			DRIVE	U		V		W
 * 	0		0	0	1		1				0		PWM	HZ
 * 	1		0	1	1		3				PWM	0		HZ
 * 	2		0	1	0		2				PWM HZ	0
 * 	3		1	1	0		6				0		HZ	PWM
 * 	4		1	0	0		4				HZ	0		PWM
 * 	5		1	0	1		5				HZ	PWM	0
 */
static const unsigned int _Hall[7] = {-1, 0, 2, 1, 4, 5, 3};

/**
 * @brief Return current velocity, based on Hall sensors
 */
float get_velocity(void){
	return _Velocity;
}

static inline int motor_control_get_phase(void){

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


void motor_control_process_event(uint64_t timestamp_us)
{
	(void)timestamp_us;

	motor_control_speed(motor_control_get_phase(),timestamp_us);
	//TODO !
	//[
	platform_gpio_set(PLATFORM_GPIO_UEN | PLATFORM_GPIO_VEN);
	platform_gpio_clear(PLATFORM_GPIO_WEN);
	//]
	//TODO !
}
