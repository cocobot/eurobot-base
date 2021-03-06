#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_TIMER

#include <stdlib.h>
#include <mcual.h>

#ifdef CONFIG_DEVICE_STM32L496xx
# include <stm32l4xx.h>
#else
# include <stm32f4xx.h>
#endif

static TIM_TypeDef * mcual_timer_get_register(mcual_timer_t timer)
{
  switch(timer)
  {
    case MCUAL_TIMER1:
      return TIM1;

    case MCUAL_TIMER2:
      return TIM2;

    case MCUAL_TIMER3:
      return TIM3;

    case MCUAL_TIMER4:
      return TIM4;

    case MCUAL_TIMER5:
      return TIM5;

    case MCUAL_TIMER6:
      return TIM6;

    case MCUAL_TIMER7:
      return TIM7;

    case MCUAL_TIMER8:
      return TIM8;

#ifdef TIM9
    case MCUAL_TIMER9:
      return TIM9;
#endif
  }

  return NULL;
}

static mcual_clock_id_t mcual_timer_get_clock(mcual_timer_t timer)
{
  switch(timer)
  {
    case MCUAL_TIMER1:
      return MCUAL_CLOCK_PERIPHERAL_2;

    case MCUAL_TIMER2:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER3:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER4:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER5:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER6:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER7:
      return MCUAL_CLOCK_PERIPHERAL_1;

    case MCUAL_TIMER8:
      return MCUAL_CLOCK_PERIPHERAL_2;

    case MCUAL_TIMER9:
      return MCUAL_CLOCK_PERIPHERAL_2;
  }

  return 0;
}

static void mcual_timer_set_clock(mcual_timer_t timer)
{
  //enable clock
  switch(timer)
  {
    case MCUAL_TIMER1:
      RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
      break;

#ifdef RCC_APB1ENR_TIM2EN
    case MCUAL_TIMER2:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      break;
#endif
#ifdef RCC_APB1ENR1_TIM2EN
    case MCUAL_TIMER2:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
      break;
#endif

#ifdef RCC_APB1ENR_TIM3EN
    case MCUAL_TIMER3:
      RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
      break;
#endif
#ifdef RCC_APB1ENR1_TIM3EN
    case MCUAL_TIMER3:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
      break;
#endif

#ifdef RCC_APB1ENR_TIM4EN
    case MCUAL_TIMER4:
      RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
      break;
#endif
#ifdef RCC_APB1ENR1_TIM4EN
    case MCUAL_TIMER4:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
      break;
#endif

#ifdef RCC_APB1ENR_TIM5EN
    case MCUAL_TIMER5:
      RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
      break;
#endif
#ifdef RCC_APB1ENR1_TIM5EN
    case MCUAL_TIMER5:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
      break;
#endif

#ifdef RCC_APB1ENR_TIM6EN
    case MCUAL_TIMER6:
      RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
      break;
#endif
#ifdef RCC_APB1ENR1_TIM6EN
    case MCUAL_TIMER6:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
      break;
#endif

#ifdef RCC_APB1ENR_TIM7EN
    case MCUAL_TIMER7:
      RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
      break;
#endif
#if RCC_APB1ENR1_TIM7EN
    case MCUAL_TIMER7:
      RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
      break;
#endif

    case MCUAL_TIMER8:
      RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
      break;

#ifdef RCC_APB2ENR_TIM9EN
    case MCUAL_TIMER9:
      RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
      break;
#endif
#if RCC_APB2ENR1_TIM9EN
    case MCUAL_TIMER9:
      RCC->APB2ENR1 |= RCC_APB2ENR1_TIM9EN;
      break;
#endif
  }
}

void mcual_timer_init(mcual_timer_t timer, int32_t freq_Hz)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);

  mcual_clock_id_t clock = mcual_timer_get_clock(timer);

  mcual_timer_set_clock(timer);
  
  if(freq_Hz > 0)
  {
#ifdef CONFIG_DEVICE_STM32L496xx
    reg->ARR = mcual_clock_get_frequency_Hz(clock) / freq_Hz;
#else
    reg->ARR = mcual_clock_get_frequency_Hz(clock) * 2 / freq_Hz;
#endif
    reg->PSC = 0;
  }
  else
  {
    reg->ARR = 0xFFFFFFFF;
    int32_t pres = -freq_Hz;
#ifdef CONFIG_DEVICE_STM32L496xx
    reg->PSC = mcual_clock_get_frequency_Hz(clock) / pres;
#else
    reg->PSC = mcual_clock_get_frequency_Hz(clock) * 2 / pres;
#endif

    reg->EGR = TIM_EGR_UG;
  }
  reg->CCMR1 = 0;
  reg->CCMR2 = 0;
  reg->CCER = 0;

  reg->CNT = 0;
  reg->CR1 = TIM_CR1_CEN;
}

void mcual_timer_init_servo(mcual_timer_t timer)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);

  mcual_clock_id_t clock = mcual_timer_get_clock(timer);

  mcual_timer_set_clock(timer);

#ifdef CONFIG_DEVICE_STM32L496xx
  reg->PSC = (mcual_clock_get_frequency_Hz(clock) / 1000000) - 1;
#else
  reg->PSC = (mcual_clock_get_frequency_Hz(clock) * 2 / 1000000) - 1;
#endif
  //50Hz
  reg->ARR = 19999;

  //Only on update
  reg->CR1 = TIM_CR1_ARPE;
  reg->EGR = TIM_EGR_UG;

  //default, nothing is activated, counter value is reset
  reg->CCMR1 = 0;
  reg->CCMR2 = 0;
  reg->CCER = 0;
  reg->CNT = 0;

  //activate
  reg->CR1 |= TIM_CR1_CEN;
}

//For 32 bits timers (TIM2/TIM5)
void mcual_timer_init_encoder(mcual_timer_t timer)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);

  mcual_timer_set_clock(timer);

  //ARR to maximum value
  reg->ARR = 0xFFFFFFFF;

  //Timer in encoder mode 3 (both edge)
  reg->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

  //Channel 1 and 2 are inputs mapped on TI1 and TI2 respectivelly
  reg->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

  //No input filter, Polarity is not inverted

  //Reset counter
  reg->CNT = 0;

  //Start the timer
  reg->CR1 = TIM_CR1_CEN;
}

uint32_t mcual_timer_get_value(mcual_timer_t timer)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);
  return reg->CNT;
}

uint32_t mcual_timer_get_timer_tick(mcual_timer_t timer)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);
  mcual_clock_id_t clock = mcual_timer_get_clock(timer);

  uint32_t f_Hz = mcual_clock_get_frequency_Hz(clock) / (reg->PSC + 1);

  return 1000000000 / f_Hz;
}

void mcual_timer_enable_channel(mcual_timer_t timer, mcual_timer_channel_t channel)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);

  if(timer == MCUAL_TIMER1)
  {
    reg->BDTR = TIM_BDTR_MOE;
  }

  if(channel & MCUAL_TIMER_CHANNEL1)
  {
    reg->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
    reg->CCER |= TIM_CCER_CC1E;
  }
  if(channel & MCUAL_TIMER_CHANNEL2)
  {
    reg->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    reg->CCER |= TIM_CCER_CC2E;
  }
  if(channel & MCUAL_TIMER_CHANNEL3)
  {
    reg->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    reg->CCER |= TIM_CCER_CC3E;
  }
  if(channel & MCUAL_TIMER_CHANNEL4)
  {
    reg->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    reg->CCER |= TIM_CCER_CC4E;
  }
}

void mcual_timer_set_duty_cycle(mcual_timer_t timer, mcual_timer_channel_t channel, uint32_t duty_cycle)
{
  if(duty_cycle >= 0x10000)
  {
    duty_cycle = 0xffff;
  }
  TIM_TypeDef * reg = mcual_timer_get_register(timer);
  uint32_t per = reg->ARR;

  duty_cycle = ((uint32_t)(duty_cycle * per)) / 0xffff;
  if(duty_cycle > per)
  {
    duty_cycle = per;
  }

  if(channel & MCUAL_TIMER_CHANNEL1)
  {
    reg->CCR1 = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL2)
  {
    reg->CCR2 = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL3)
  {
    reg->CCR3 = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL4)
  {
    reg->CCR4 = duty_cycle;
 }
}

#endif

