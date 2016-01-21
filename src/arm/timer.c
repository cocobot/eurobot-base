#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_TIMER

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

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

    case MCUAL_TIMER9:
      return TIM9;
  }

  return NULL;
}

void mcual_timer_init(mcual_timer_t timer, uint32_t freq_Hz)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);

  reg->ARR = mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1) / freq_Hz;
  reg->CNT = 0;
  reg->CR1 = TIM_CR1_CEN;
}

void mcual_timer_enable_channel(mcual_timer_t timer, mcual_timer_channel_t channel)
{
  TIM_TypeDef * reg = mcual_timer_get_register(timer);
  if(channel & MCUAL_TIMER_CHANNEL1)
  {
    reg->CCMR1 |= TIM_CCMR1_OC1M_0;
    reg->CCER |= TIM_CCER_CC1E;
  }
  if(channel & MCUAL_TIMER_CHANNEL2)
  {
    reg->CCMR1 |= TIM_CCMR1_OC2M_0;
    reg->CCER |= TIM_CCER_CC2E;
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

