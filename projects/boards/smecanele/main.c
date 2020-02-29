#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>


static void thread(void * arg)
{
  (void)arg;

  while(1)
  {
    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
  } 
}

void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  switch(pid)
  {
    case COCOBOT_COM_SET_SERVO_PID:
      {
        uint8_t id;
        int32_t value;
        uint32_t offset = 0;

        offset += cocobot_com_read_B(data, len, offset, &id);
        offset += cocobot_com_read_D(data, len, offset, &value);
        platform_servo_set_value(id, value);

        cocobot_com_printf("Servo %lu en position %lu", id, value);
      }
      break;
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

  cocobot_com_init(MCUAL_USART1, 1, 1, com_handler);

  xTaskCreate(thread, "thread", 1024, NULL, 3, NULL);
  vTaskStartScheduler();

  return 0;
}
