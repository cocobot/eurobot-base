#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_ADC

#include <mcual.h>


void mcual_adc_init(mcual_adc_id_t id)
{

}

int32_t mcual_adc_get(mcual_adc_id_t id)
{
  (void)id;

  return 42;
}

#endif
