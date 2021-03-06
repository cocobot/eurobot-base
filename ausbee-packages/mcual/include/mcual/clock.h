#ifndef MCUAL_CLOCK_H
#define MCUAL_CLOCK_H

#include <stdint.h>

#define MCUAL_CLOCK_PLL_DISABLED (-1)

typedef enum
{
  MCUAL_CLOCK_SOURCE_INTERNAL,
  MCUAL_CLOCK_SOURCE_EXTERNAL,
} mcual_clock_source_t;

typedef enum
{
  MCUAL_CLOCK_SYSTEM,
  MCUAL_CLOCK_PERIPHERAL_1,
  MCUAL_CLOCK_PERIPHERAL_2,
} mcual_clock_id_t;

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz);
uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id);

#endif// MCUAL_CLOCK_H
