#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <vl53l0x.h>

static int toto = 0;

void update_lcd(void * arg)
{
    (void)arg;

    float distance = 300;
    //int32_t zero = cocobot_position_get_distance();
    while(1)
    {
        //toggle led
        vTaskDelay(100 / portTICK_PERIOD_MS);
        distance = vl53l0x_get_distance_mm(toto);
        if(distance > 300)
        {
            platform_led_set(PLATFORM_LED_BLUE);
            platform_led_clear(PLATFORM_LED_GREEN);
            platform_led_clear(PLATFORM_LED_ORANGE);
        }
        else if(distance < 300)
        {
            platform_led_set(PLATFORM_LED_GREEN);
            platform_led_clear(PLATFORM_LED_BLUE);
            platform_led_clear(PLATFORM_LED_ORANGE);
        }
        else if(distance == 300)
        {
            platform_led_set(PLATFORM_LED_ORANGE);
            platform_led_clear(PLATFORM_LED_BLUE);
            platform_led_clear(PLATFORM_LED_GREEN);
        }

        platform_led_toggle(PLATFORM_LED_RED);
    }
}


int main(int argc, char *argv[]) 
{
#ifdef AUSBEE_SIM
    mcual_arch_main(argc, argv);
#else
    (void)argc;
    (void)argv;
#endif
    platform_init();
    vl53l0x_init(3);
    toto = vl53l0x_register_device(0x53);


    xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

    vTaskStartScheduler();

    return 0;
}
