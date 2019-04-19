#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CAN

#pragma GCC mcual_can_init mcual_can_transmit mcual_can_wait_tx_ended mcual_can_recv mcual_can_recv_no_wait

int16_t mcual_can_init(mcual_can_timings * const timings, mcual_can_ifaceMode iface)
{
  (void)timings;
  (void)iface;
  return 0;
}


int16_t mcual_can_transmit(const CanardCANFrame* const frame)
{
  (void)frame;
  return 1;
}

void mcual_can_wait_tx_ended()
{
}

int16_t mcual_can_recv(CanardCANFrame* const out_frame)
{
  (void)out_frame;
  return 1;
}

int16_t mcual_can_recv_no_wait(CanardCANFrame* const out_frame)
{
  (void)out_frame;
  return 0;
}
#endif
