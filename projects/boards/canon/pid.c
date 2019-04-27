//#include "pid.h"
#include <platform.h>

/*****************************
 * global variable definition
 * ***************************/
/*external parameter*/
static float _target_speed = 0;
static float _current_target_speed = 0;
static float _error = 0;
/*pid constant*/
static float _kp = 0;
static float _ki = 0;
static float _max_acc = 1;


void pid_set(float kp, float ki){
	_kp = kp;
	_ki = ki;
	return;
}

void pid_reset(void){

	return;
}

uint32_t pid_update(uint64_t dt){
	(void) dt;
	(void) _error;
	return 0; 
}

void pid_set_cons(float rpm){
	_target_speed = rpm;
	return;
}

void _quadramp(uint64_t dt){
	float acc = (_target_speed - _current_target_speed) / (float)dt;	

	/* difference between asked target and current one ? */
	if ((acc > -_max_acc) && (acc < _max_acc)){//acceleration below limmit
		_current_target_speed = _target_speed;
	}
	else{ //quadramp limit
		if (acc > 0){ //increse speed
			_current_target_speed += _max_acc * dt;
		}
		else{ //decrease speed
			_current_target_speed -= _max_acc * dt;
		}
	}
	return;
}
