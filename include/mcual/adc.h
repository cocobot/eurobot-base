#ifndef MCUAL_ADC_H
#define MCUAL_ADC_H

#include <stdint.h>

typedef uint32_t mcual_adc_id_t;

#define MCUAL_ADC0 ((mcual_adc_id_t) 0)
#define MCUAL_ADC1 ((mcual_adc_id_t) 1)

void mcual_adc_init(mcual_adc_id_t);
int32_t mcual_adc_get(mcual_adc_id_t);


#endif// MCUAL_ADC_H
