#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_ADC

#include <stm32f4xx.h>
#include <mcual.h>


void mcual_adc_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  ADC->CCR = ADC_CCR_ADCPRE_1 | ADC_CCR_ADCPRE_0;
  ADC1->CR1 = 0x00;
  ADC1->CR2 = ADC_CR2_ADON;
}

int32_t mcual_adc_get(mcual_adc_id_t id)
{
  (void)id;

  ADC1->JSQR = (id << 15) & ADC_JSQR_JSQ4; 
  ADC1->SR &= ~ADC_SR_JEOC;
  ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_JSWSTART;

  while(!(ADC1->SR & ADC_SR_JEOC));
  return ADC1->JDR1;
}

#endif
