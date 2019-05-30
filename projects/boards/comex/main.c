#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>
#include <math.h>

//palm value
#define COMEX_DISABLE_PALM_VAL 2000
#define COMEX_ENABLE_PALM_VAL 3500
#define COMEX_PALM_AMPL_VAL 1000
#define COMEX_PALM_INCR 15
#define COMEX_PALM_OSC_INCR 25


static volatile int _enable = 0;

static void _run_palm_tree(void * arg){
	static uint16_t val = COMEX_DISABLE_PALM_VAL;

	static int state = 0;

	(void) arg;

	int test = 0;
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
		for(int i = 0; i < 20; i += 1)
		{
			vTaskDelay(5/portTICK_PERIOD_MS);
			if(test > i)
			{
				mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
				test ^= 0xA;
			}
		}

		test += 1;
	}
	return;
}

void wait(void * arg)
{
	(void)arg;


	mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
	mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
	int value = 0;

	while(1)
	{
		if(!mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN0))
		{
			value += 1;
		}
		else
		{
			value -= 5;
		}

		if(value < 0)
		{
			value = 0;
		}
		if(value > 200)
		{
			break;
		}

		vTaskDelay(50/portTICK_PERIOD_MS);
	}

	mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);

	while(1)
	{
		if(mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN0))
		{
			value += 1;
		}
		else
		{
			value -= 5;
		}

		if(value < 0)
		{
			value = 0;
		}
		if(value > 500)
		{
			break;
		}

		vTaskDelay(50/portTICK_PERIOD_MS);
	}



	mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
	xTaskCreate(_run_palm_tree,"palm", 1024, NULL, 4, NULL);

	_enable = 1;

	while(1)
	{
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

int main(void) 
{
	platform_init();

	//Remove com from now
	//cocobot_com_init();
	//cocobot_com_run();

	mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN13, MCUAL_GPIO_OUTPUT);
 	mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN13);

	mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);


	//wait for detection
	//
	
	xTaskCreate(wait, "wait", 1024, NULL, 1, NULL);


	vTaskStartScheduler();
	vTaskSwitchContext();


	return 0;
}
