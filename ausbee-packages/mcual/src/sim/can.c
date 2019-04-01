#include <mcual.h>
#include <FreeRTOS.h>
#include <queue.h>

static QueueHandle_t can_rx_queue;

int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
  (void)timings;
  (void)iface;
  can_rx_queue = xQueueCreate(CONFIG_MCUAL_CAN_RX_SIZE, sizeof(CanardCANFrame));
  return 0;
}

int16_t mcual_can_transmit(const CanardCANFrame* const frame)
{
  mcual_arch_request("CAN", 1, "%08x:%01x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                     frame->id,
                     frame->data_len,
                     frame->data[0],
                     frame->data[1],
                     frame->data[2],
                     frame->data[3],
                     frame->data[4],
                     frame->data[5],
                     frame->data[6],
                     frame->data[7]
                     );
  return 1;
}

void mcual_can_wait_tx_ended()
{
}

int16_t mcual_can_recv_no_wait(CanardCANFrame* const out_frame)
{
  if(xQueueReceive(can_rx_queue, out_frame, 0) == pdFALSE)
  {
    return 0;
  }
  return 1;
}

void mcual_can_recv_new_frame(CanardCANFrame * frame)
{
  xQueueSend(can_rx_queue, frame, 0);
}
