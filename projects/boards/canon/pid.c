//#include "pid.h"
#include <platform.h>

void pid_set(float kp, float ki){
	(void) kp;
	(void) ki;

	return;
}

void pid_reset(void){

	return;
}

uint32_t pid_update(void){
	return 0; 
}

void pid_set_cons(float rpm){
	(void)rpm;
	return;
}



