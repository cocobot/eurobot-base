#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_ADC

#include <stm32l4xx.h>
#include <mcual.h>

#pragma GCC mcual_adc_get mcual_adc_init

void mcual_adc_init(void)
{
}

int32_t mcual_adc_get(mcual_adc_id_t id)
{
  (void)id;
  return ADC1->JDR1;
}

#endif
