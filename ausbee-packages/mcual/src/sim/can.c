#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#include <stdio.h>
#include <mcual.h>
#include <FreeRTOS.h>
#include <queue.h>

static QueueHandle_t can_rx_queue;

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

void mcual_can_recv_new_frame(mcual_can_frame_t * frame)
{
  xQueueSend(can_rx_queue, frame, 0);
}
#endif
