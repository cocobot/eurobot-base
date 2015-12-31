#include <mcual.h>



void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  (void)source;
  (void)target_freq_kHz;

  //create thread for simulated peripherals
  // TODO: is this line really needed?
  //mcual_arch_sim_init_peripherals();
}
