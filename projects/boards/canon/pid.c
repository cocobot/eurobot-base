#include "pid.h"
#include <platform.h>


/*if set, integral does not accumulate when in speed limit*/
#define PID_INTEGRAL_LIMIT 1 

/*****************************
 * global variable definition
 * ***************************/
/*external parameter*/
static float _target_speed = 0.0;
static float _error = 0.0;
/*pid constant*/
static float _kp = 0.0;
static float _ki = 0.0;
static float _kd = 0.0;
static float _max_acc = 1.0;
static float _max_speed = 200.0;
/*flags*/
static int _quad_limit = 0;
static int _speed_limit = 0;


/*******************************
 * Private function declaration
 * *****************************/
static inline float _quadramp(float target_speed, float current_speed, uint64_t dt);
static float _pid_filter(float error, uint64_t dt, int reset);
static inline float _limit_out(float speed);

	

/********************************
 * Public function definition
 * *****************************/

void pid_set(float kp, float ki, float kd){
	_kp = kp;
	_ki = ki;
	_kd = kd;
	return;
}

void pid_set_limit(float speed_limit, float quad_limit){
	_speed_limit = speed_limit;
	_quad_limit = quad_limit;
	return;
}

void pid_reset(void){
	_error = 0.0; //reset currrent error
	_pid_filter(0.0, 0, 1);
	return;
}

int pid_is_limited(void){
	return (_quad_limit << 1) | _speed_limit;
}

float pid_update(float velocity, uint64_t dt){
	float out = 0.0;

	/* Compute new error - quadramp limitation*/
	_error = _quadramp(_target_speed - velocity, _error, dt);
	
	/*compute pid filter*/
	out = _pid_filter(_error, dt, 0);	
	
	/*limit speed*/
	out = _limit_out(out);
	
	return out; 
}

void pid_set_cons(float rpm){
	_target_speed = rpm;
	return;
}


/******************************
 * Private function definition
 * ****************************/
static inline float _quadramp(float target_speed, float current_speed, uint64_t dt){
	float acc = (target_speed - current_speed) / (float)dt;	

	/* difference between asked target and current one ? */
	if ((acc > -_max_acc) && (acc < _max_acc)){//acceleration below limmit
		_quad_limit = 0;
		return target_speed;
	}
	else{ //quadramp limit
		_quad_limit = 1;
		if (acc > 0){ //increse speed
			 return current_speed + _max_acc * (float)dt;
		}
		else{ //decrease speed
			return current_speed - _max_acc * (float)dt;
		}
	}
	return 0.0; //never reached
}

static float _pid_filter(float error, uint64_t dt, int reset){
	static float last_error = 0.0;
	static float integ_error = 0.0;
	float p,i,d;

	if (reset == 1){ // reset asked
		last_error = 0.0;
		integ_error = 0.0;
		return 0.0;
	}

	/* Compute pid filter*/
	//proportional
 	p = _kp * error; 

	//integral 
#if PID_INTEGRAL_LIMIT	
	if (! _speed_limit){//not in speed limitation
		i = (integ_error += _ki * error * (float)dt); 
	}
	else {
		i = integ_error;
	}
#else
	i = (integ_error += _ki * error * (float)dt); 
#endif

	//derivative
	if (dt > 0){ //just to be safe
		d = _kd * (error - last_error) / (float)dt; 
		last_error = error;
	}
	else {
		d = 0.0;
	}

	return p + i + d;
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
