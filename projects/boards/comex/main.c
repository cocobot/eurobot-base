#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>

//palm value
#define COMEX_DISABLE_PALM_VAL 600
#define COMEX_ENABLE_PALM_VAL 1000
#define COMEX_PALM_AMPL 100
#define COMEX_PALM_INCR 5
#define COMEX_PALM_OSC_INCR 5

#define COMEX_WAVE_PWM 1


static volatile int enable = 0.0;

static void _run_palm_tree(void * arg){
	static uint16_t val = COMEX_DISABLE_PALM_VAL;
	static int osc = 0;

	(void) arg;

	for (;;){
		if (enable){
			if (!osc){
				if (val < COMEX_ENABLE_PALM_VAL){
					val += COMEX_PALM_INCR;
				}
				else{
					osc = -1;
				}
			}
			else{
				if (val < COMEX_DISABLE_PALM_VAL - COMEX_PALM_AMPL){
					osc = 1;
				}
				else if (val > COMEX_ENABLE_PALM_VAL)
					osc = -1;
			}
			val += osc * COMEX_PALM_OSC_INCR;
		}

		platform_servo_set_value(PLATFORM_SERVO_0, val);
		vTaskDelay(10000/portTICK_PERIOD_MS);
	}
	return;
}

static vois _run_wave(void * arg){
	(void) arg;
	
}



// arg  550 .. 2450
static void servo(void * arg)
{
	(void)arg;
	static uint32_t value = 550;
	while(1)
	{
		platform_servo_set_value(PLATFORM_SERVO_0, value);
		platform_servo_set_value(PLATFORM_SERVO_1, value);
		value += 100;
		if(value > 2450)
			value = 550;
		vTaskDelay(100/portTICK_PERIOD_MS);
	}

}

static void blink(void * arg)
{
	(void)arg;
	while(1)
	{
		platform_led_toggle(PLATFORM_LED_RED_5);
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

int main(void) 
{
	platform_init();

	//Remove com from now
	//cocobot_com_init();
	//cocobot_com_run();

	xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);
	xTaskCreate(servo, "servo", 1024, NULL, 1, NULL);

	vTaskStartScheduler();
	vTaskSwitchContext();

	return 0;
}
