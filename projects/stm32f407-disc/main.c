#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
//#include <cocobot.h>


void update_lcd(void * arg)
{
    (void)arg;

    //int32_t zero = cocobot_position_get_distance();
    while(1)
    {
        //toggle led
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

    vTaskStartScheduler();

    return 0;
}
