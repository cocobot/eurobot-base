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
        case 0: //Init
          cocobot_arm_action_init();
          break;
        case 1: //Front Up
          cocobot_arm_action_pliers_up(PLIERS_CARRIER_FRONT);
          break;
        case 2: //Front Down
          cocobot_arm_action_pliers_down(PLIERS_CARRIER_FRONT);
          break;
        case 3: //F all open
          cocobot_arm_action_pliers_open(PLIERS_FRONT_LEFT);
          cocobot_arm_action_pliers_open(PLIERS_FRONT_RIGHT);
          cocobot_arm_action_pliers_open(PLIERS_FRONT_CENTER);
          break;
        case 4: //F all close
          cocobot_arm_action_pliers_close(PLIERS_FRONT_LEFT);
          cocobot_arm_action_pliers_close(PLIERS_FRONT_RIGHT);
          cocobot_arm_action_pliers_close(PLIERS_FRONT_CENTER);
          break;
        case 5: //F left O
          cocobot_arm_action_pliers_open(PLIERS_FRONT_LEFT);
          break;
        case 6: //F left C 
          cocobot_arm_action_pliers_close(PLIERS_FRONT_LEFT);
          break;
        case 7: //F center O
          cocobot_arm_action_pliers_open(PLIERS_FRONT_CENTER);
          break;
        case 8: //F center C 
          cocobot_arm_action_pliers_close(PLIERS_FRONT_CENTER);
          break;
        case 9: //F right O
          cocobot_arm_action_pliers_open(PLIERS_FRONT_RIGHT);
          break;
        case 10: //F right C 
          cocobot_arm_action_pliers_close(PLIERS_FRONT_RIGHT);
          break;
        case 11: //Back Up
          cocobot_arm_action_pliers_up(PLIERS_CARRIER_BACK);
          break;
        case 12: //Back Down
          cocobot_arm_action_pliers_down(PLIERS_CARRIER_BACK);
          break;
        case 13: //B all O
          cocobot_arm_action_pliers_open(PLIERS_BACK_LEFT);
          cocobot_arm_action_pliers_open(PLIERS_BACK_RIGHT);
          cocobot_arm_action_pliers_open(PLIERS_BACK_CENTER);
          break;
        case 14: //B all close
          cocobot_arm_action_pliers_close(PLIERS_BACK_LEFT);
          cocobot_arm_action_pliers_close(PLIERS_BACK_RIGHT);
          cocobot_arm_action_pliers_close(PLIERS_BACK_CENTER);
          break;
        case 15: //B left O
          cocobot_arm_action_pliers_open(PLIERS_BACK_LEFT);
          break;
        case 16: //B left C 
          cocobot_arm_action_pliers_close(PLIERS_BACK_LEFT);
          break;
        case 17: //B center O
          cocobot_arm_action_pliers_open(PLIERS_BACK_CENTER);
          break;
        case 18: //B center C 
          cocobot_arm_action_pliers_close(PLIERS_BACK_CENTER);
          break;
        case 19: //B right O
          cocobot_arm_action_pliers_open(PLIERS_BACK_RIGHT);
          break;
        case 20: //B right C 
          cocobot_arm_action_pliers_close(PLIERS_BACK_RIGHT);
          break;
        case 21: //L Windsock down
          cocobot_arm_action_windsock_down(WINDSOCK_LEFT);
          break;
        case 22: //L Windsock up
          cocobot_arm_action_windsock_up(WINDSOCK_LEFT);
          break;
        case 23: //R Windsock down
          cocobot_arm_action_windsock_down(WINDSOCK_RIGHT);
          break;
        case 24: //R Windsock up
          cocobot_arm_action_windsock_up(WINDSOCK_RIGHT);
          break;
        case 25: //Flag up
          cocobot_arm_action_flag_up();
          break;
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
