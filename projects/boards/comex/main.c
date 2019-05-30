#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>

//palm value
#define COMEX_DISABLE_PALM_VAL 2000
#define COMEX_ENABLE_PALM_VAL 3500
#define COMEX_PALM_AMPL_VAL 1000
#define COMEX_PALM_INCR 15
#define COMEX_PALM_OSC_INCR 25

#define COMEX_WAVE_PWM 1


static volatile int _enable = 0;

#if 1
static void _run_palm_tree(void * arg){
	static uint16_t val = COMEX_DISABLE_PALM_VAL;

	static int state = 0;

	(void) arg;

	for (;;){
		switch (state){
			case 0 :
				if (_enable){
					state = 1;
				}
				break;
			case 1 :
				val += COMEX_PALM_INCR;
				if (val > COMEX_ENABLE_PALM_VAL){
					state = 2;
				}
				break;
			case 2 :
				val += COMEX_PALM_OSC_INCR;
				if (val > COMEX_ENABLE_PALM_VAL + COMEX_PALM_AMPL_VAL){
					val = COMEX_ENABLE_PALM_VAL + COMEX_PALM_AMPL_VAL;
					state = 3;
				}
				break;
			case 3 :
				val -= COMEX_PALM_OSC_INCR;
				if (val < COMEX_ENABLE_PALM_VAL){
					val = COMEX_ENABLE_PALM_VAL;
					state = 2;
				}
				break;
			default:
				break;
		}

		platform_servo_set_value(PLATFORM_SERVO_0, val);
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
	return;
}
#endif

/*
	 static void _run_wave(void * arg){
	 (void) arg;

	 }
	 */


#if 0
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

#endif

static volatile int blink_thr = 0;

static void blink(void * arg)
{
	int i;

	(void)arg;
	while(1)
	{
				for (i = 0; i <30; i++){
					if (i <  blink_thr){
						mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
					}
					else{
mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
					}

				}
				vTaskDelay(portTICK_PERIOD_MS);
	}
}

static void refresh(void * arg){
	int dir = 1;

	(void) arg;
	
	for (;;){
		blink_thr =+ dir;
		if (blink_thr >= 30){
			dir = -1;
		}
		if (blink_thr <= 1){
			dir = 1;
		}
		vTaskDelay(200/portTICK_PERIOD_MS);
	}
}

int main(void) 
{
	platform_init();

	//Remove com from now
	//cocobot_com_init();
	//cocobot_com_run();

	xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);
	//	xTaskCreate(servo, "servo", 1024, NULL, 1, NULL);
	xTaskCreate(_run_palm_tree,"palm", 1024, NULL, 1, NULL);
	xTaskCreate(refresh,"refresh", 1024, NULL, 1, NULL);
	mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
 	mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
	//	pb12
//	platform_gpio_set(PLATFORM_LED_RED_1);

	_enable = 1;
	vTaskStartScheduler();
	vTaskSwitchContext();


	return 0;
}
