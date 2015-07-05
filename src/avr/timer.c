#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_TIMER

#include <stdlib.h>
#include <mcual.h>
#include <avr/io.h>

static TC0_t * mcual_timer_get_register(mcual_timer_t timer)
{
  switch(timer)
  {
    case MCUAL_TIMER0:
      return &TCC0;

    case MCUAL_TIMER1:
      return (TC0_t *)&TCC1;

    case MCUAL_TIMER2:
      return &TCD0;

    case MCUAL_TIMER3:
      return (TC0_t *)&TCD1;

    case MCUAL_TIMER4:
      return &TCE0;

    case MCUAL_TIMER5:
      return (TC0_t *)&TCE1;

    case MCUAL_TIMER6:
      return &TCF0;

    case MCUAL_TIMER7:
      return (TC0_t *)&TCF1;
  }

  return NULL;
}

void mcual_timer_init(mcual_timer_t timer, uint32_t freq_Hz)
{
  TC0_t * reg = mcual_timer_get_register(timer);
  
  uint32_t period = mcual_clock_get_frequency_Hz(MCUAL_CLOCK_SYSTEM) / freq_Hz;

  if(period < 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV1_gc;
    reg->PER = period;
  }
  else if(period < 2 * 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV2_gc;
    reg->PER = period / 2;
  }
  else if(period < 4 * 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV4_gc;
    reg->PER = period / 4;
  }
  else if(period < 8 * 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV8_gc;
    reg->PER = period / 8;
  }
  else if(period < 64 * 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV64_gc;
    reg->PER = period / 64;
  }
  else if(period < 256 * 0x10000)
  {
    reg->CTRLA = TC_CLKSEL_DIV256_gc;
    reg->PER = period / 256;
  }
  else
  {
    reg->CTRLA = TC_CLKSEL_DIV1024_gc;
    reg->PER = period / 1024;
  }

  reg->CTRLB = TC_WGMODE_SS_gc;
}

void mcual_timer_enable_channel(mcual_timer_t timer, mcual_timer_channel_t channel)
{
  TC0_t * reg = mcual_timer_get_register(timer);
  uint8_t ctrlb = 0;
  if(channel & MCUAL_TIMER_CHANNEL0)
  {
    ctrlb |= TC0_CCAEN_bm;
  }
  if(channel & MCUAL_TIMER_CHANNEL1)
  {
    ctrlb |= TC0_CCBEN_bm;
  }
  if(channel & MCUAL_TIMER_CHANNEL2)
  {
    ctrlb |= TC0_CCCEN_bm;
  }
  if(channel & MCUAL_TIMER_CHANNEL3)
  {
    ctrlb |= TC0_CCDEN_bm;
  }
  reg->CTRLB |= ctrlb;
}

void mcual_timer_set_duty_cycle(mcual_timer_t timer, mcual_timer_channel_t channel, uint32_t duty_cycle)
{
  if(duty_cycle >= 0x10000)
  {
    duty_cycle = 0xffff;
  }
  TC0_t * reg = mcual_timer_get_register(timer);
  uint32_t per = reg->PER;

  duty_cycle = ((uint32_t)(duty_cycle * per)) / 0xffff;

  if(channel & MCUAL_TIMER_CHANNEL0)
  {
    reg->CCA = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL1)
  {
    reg->CCB = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL2)
  {
    reg->CCC = duty_cycle;
  }
  if(channel & MCUAL_TIMER_CHANNEL3)
  {
    reg->CCD = duty_cycle;
  }
}

#endif
