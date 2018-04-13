#ifndef MCUAL_ADC_H
#define MCUAL_ADC_H

#include <stdint.h>

typedef uint32_t mcual_adc_id_t;

#define MCUAL_ADC0  ((mcual_adc_id_t)  0)
#define MCUAL_ADC1  ((mcual_adc_id_t)  1)
#define MCUAL_ADC2  ((mcual_adc_id_t)  2)
#define MCUAL_ADC3  ((mcual_adc_id_t)  3)
#define MCUAL_ADC4  ((mcual_adc_id_t)  4)
#define MCUAL_ADC5  ((mcual_adc_id_t)  5)
#define MCUAL_ADC6  ((mcual_adc_id_t)  6)
#define MCUAL_ADC7  ((mcual_adc_id_t)  7)
#define MCUAL_ADC8  ((mcual_adc_id_t)  8)
#define MCUAL_ADC9  ((mcual_adc_id_t)  9)
#define MCUAL_ADC10 ((mcual_adc_id_t) 10)
#define MCUAL_ADC11 ((mcual_adc_id_t) 11)
#define MCUAL_ADC12 ((mcual_adc_id_t) 12)
#define MCUAL_ADC13 ((mcual_adc_id_t) 13)
#define MCUAL_ADC14 ((mcual_adc_id_t) 14)
#define MCUAL_ADC15 ((mcual_adc_id_t) 15)

void mcual_adc_init(void);
int32_t mcual_adc_get(mcual_adc_id_t id);


#endif// MCUAL_ADC_H
