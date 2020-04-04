#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#include <stdio.h>
#include <mcual.h>
#include <FreeRTOS.h>
#include <queue.h>

static QueueHandle_t can_rx_queue;

void mcual_can_recv_from_network(const mcual_can_frame_t * const frame)
{
  xQueueSendFromISR(can_rx_queue, frame, NULL);
}

int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
  (void)timings;
  (void)iface;
  can_rx_queue = xQueueCreate(CONFIG_MCUAL_CAN_RX_SIZE, sizeof(mcual_can_frame_t));
  return 0;
}

int16_t mcual_can_transmit(const mcual_can_frame_t* const frame)
{
  mcual_arch_sim_handle_can_peripheral_write(frame);
  return 1;
}

void mcual_can_wait_tx_ended()
{
}

int16_t mcual_can_recv_no_wait(mcual_can_frame_t* const out_frame)
{
  if(xQueueReceive(can_rx_queue, out_frame, 0) == pdFALSE)
  {
    return 0;
  }
  return 1;
}

int16_t mcual_can_recv(mcual_can_frame_t * const out_frame)
{
#ifdef CONFIG_MCUAL_CAN_USE_FREERTOS_QUEUES
    xQueueReceive(can_rx_queue, out_frame, portMAX_DELAY);
#else
    int16_t r = 0;
    while(!r)
    {
        r = mcual_can_recv_no_wait(out_frame);
    }
#endif
    //todo
    return 1;
}

#endif
