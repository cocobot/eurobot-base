#include <mcual.h>


static uint32_t _freq_Hz;

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  (void)source;

  _freq_Hz = target_freq_kHz;

  //create thread for simulated peripherals
  mcual_arch_sim_init_peripherals();
}

uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id)
{
  (void)clock_id;

  //fake result
  return _freq_Hz;
}
