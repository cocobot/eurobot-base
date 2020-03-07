#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include <queue.h>
#include "cocobot_arm_action.h"

#define MECANELE_MAX_ORDER  32


static QueueHandle_t order_queue;
static uint8_t ready;

static void thread(void * arg)
{
  (void)arg;
  uint8_t order;

  order_queue = xQueueCreate(MECANELE_MAX_ORDER, sizeof(uint8_t));
  cocobot_arm_action_init();
  ready = 1;

  while(1)
  {
    if(xQueueReceive(order_queue, &order, 100 * 50 / portTICK_PERIOD_MS) == pdTRUE)
    {
      cocobot_com_printf("RUN smecanele order %lu", order);
      switch(order)
      {
        //TODO VINCENT !
      }
    }
    else
    {
      cocobot_com_printf("smecanele is inactive !!");
      //toggle led when inactive
      platform_led_toggle(PLATFORM_LED0);
    }
  } 
}

void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  if(!ready)
  {
    return;
  }

  switch(pid)
  {
    case COCOBOT_COM_MECA_ACTION_PID:
      {
        //new order for Meca !
        uint8_t id;
        cocobot_com_read_B(data, len, 0, &id);
        xQueueSend(order_queue, &id, portMAX_DELAY);
      }
      break;

    case COCOBOT_COM_SET_SERVO_PID:
      {
        //set servo position from COCOUI
        uint8_t id;
        int32_t value;
        uint32_t offset = 0;

        offset += cocobot_com_read_B(data, len, offset, &id);
        offset += cocobot_com_read_D(data, len, offset, &value);
        platform_servo_set_value(id, value);

        cocobot_com_printf("set smecanele servo %u to %lu", id, value);
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
