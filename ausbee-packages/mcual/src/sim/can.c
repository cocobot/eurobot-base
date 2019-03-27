#include <mcual.h>

int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
  (void)timings;
  (void)iface;
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
  return 0;
}
