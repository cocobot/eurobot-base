#include <stm32f4xx.h>
#include <mcual.h>

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  double source_freq_kHz = 0;

  //set internal clock as source for confiuration
  RCC->CFGR &= ~((uint32_t)((1 << 0) | (1 << 1)));

  //disable PLL
  RCC->CR &= ~((uint32_t)(1 << 24));

  switch(source)
  {
    case MCUAL_CLOCK_SOURCE_INTERNAL:
      {
        //set internal clock ON
        RCC->CR |= (uint32_t)(1 << 0);

        source_freq_kHz = 16000;
      }
      break;

    case MCUAL_CLOCK_SOURCE_EXTERNAL:
      {
        //set external clock ON
        RCC->CR |= (uint32_t)(1 << 16);

        //wait until external clock is stable
        while(!(RCC->CR & (uint32_t)(1 << 17)));

        //set external clock as main clock
        RCC->CFGR |= (uint32_t)(1 << 0);

        source_freq_kHz = HSE_VALUE / 1000; 
      }
      break;
  }

  if(target_freq_kHz > 0)
  {
    //set VCO input frequency to 2Mhz (or lower)
    int32_t pll_m = (source_freq_kHz / 2001)+1;

    int32_t pll_n = 1;
    int32_t pll_p = 1;

    //find best pll_n/pll_p coef for the requested main clock frequency
    int i;
    int32_t distance = -1;
    for(i = 0; i < 4; i += 1)
    {
      int32_t i_pll_p = (i + 1) * 2;
      int32_t i_fpll = target_freq_kHz * i_pll_p;

      //VCO must be between 192Mhz and 432Mhz
      if((i_fpll >= 192000) && (i_fpll <= 432000))
      {
        int32_t i_pll_n = (i_fpll * pll_m / (source_freq_kHz));

        int32_t real_target_kHz = (source_freq_kHz * i_pll_n) / (i_pll_p * pll_m);

        int32_t i_distance = target_freq_kHz - real_target_kHz;
        if(i_distance < 0)
        {
          i_distance = -i_distance;
        }

        if((distance < 0) || (i_distance < distance))
        {
          distance = i_distance;
          pll_n = i_pll_n;
          pll_p = i_pll_p;
        }
      }
    }

    //find pll q in order to have a 48Mhz output
    int32_t fvco = (source_freq_kHz * pll_n) / pll_m;
    int32_t pll_q = fvco / 48001 + 1;

    //set pll register
    RCC->PLLCFGR = ((uint32_t)((pll_q & 0x0F) << 24) | (((pll_p / 2 - 1) & 0x03) << 16) | ((pll_n & 0x1FF) << 6) | (pll_m & 0x3F));

    if(source == MCUAL_CLOCK_SOURCE_EXTERNAL)
    {
      RCC->PLLCFGR |= ((uint32_t)(1 << 22));
    }

    //enable PLL
    RCC->CR = ((uint32_t)(1 << 24));
    
    //wait until PLL is locked
    while(!(RCC->CR & (uint32_t)(1 << 25)));

    //set PLL output as main clock
    RCC->CFGR &= ~((uint32_t)((1 << 0) | (1 << 1)));
    RCC->CFGR |= (uint32_t)(1 << 1);
  }
  else
  {
    target_freq_kHz = source_freq_kHz;
  }

  int32_t i;

  //set Prescaler1
  int32_t pre1 = 2;
  for(i = 0; i < 4; i += 1)
  {
    if((target_freq_kHz / (pre1 << i)) < 45000)
    {
      pre1 = i;
      break;
    }
  }
  pre1 |= (1 << 3);
  
  int32_t pre2 = 2;
  for(i = 0; i < 4; i += 1)
  {
    if((target_freq_kHz / (pre2 << i)) < 90000)
    {
      pre2 = i;
      break;
    }
  }
  pre2 |= (1 << 2);

  RCC->CFGR &= ~((uint32_t)(0x3F << 10));
  RCC->CFGR |= ((uint32_t)((pre1 << 10) | (pre2 << 13)));

  //Enable all GPIO clocks
  RCC->AHB1ENR |= 0x3FF;
}
