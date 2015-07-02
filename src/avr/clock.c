#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CLOCK

#include <mcual.h>

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  (void)source;
  (void)target_freq_kHz;
}

uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id)
{
  (void)clock_id;
  return HSE_VALUE;
}

#endif
