#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CLOCK

#if 0
#define APB1_CLOCK_MAX_FREQUECY_KHZ 45000
#define APB2_CLOCK_MAX_FREQUECY_KHZ 90000
#elif defined(STM32F40_41xxx)
# define APB1_CLOCK_MAX_FREQUECY_KHZ 42000
# define APB2_CLOCK_MAX_FREQUECY_KHZ 84000
#else
# error Architecture not supported
#endif

#include <stm32f4xx.h>
#include <mcual.h>

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  int32_t source_freq_kHz = 0;
  uint32_t cfgr = RCC->CFGR;

  //set internal clock as source for confiuration
  cfgr &= ~RCC_CFGR_SW;

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
        cfgr |= (uint32_t)(1 << 0);

        source_freq_kHz = HSE_VALUE / 1000; 
      }
      break;
  }

  if(target_freq_kHz > 0)
  {
    //set flash wait states
    uint32_t wait = target_freq_kHz / 30000;
    uint32_t acr = FLASH->ACR;
    acr &= ~FLASH_ACR_LATENCY;
    acr |= (wait & FLASH_ACR_LATENCY);
    FLASH->ACR = acr;

    
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
    else
    {
      RCC->PLLCFGR &= ~((uint32_t)(1 << 22));
    }

    //enable PLL
    RCC->CR = ((uint32_t)(1 << 24));
    
    //wait until PLL is locked
    while(!(RCC->CR & (uint32_t)(1 << 25)));

    //set PLL output as main clock
    cfgr &= ~RCC_CFGR_SW;
    cfgr |= RCC_CFGR_SW_PLL;
  }
  else
  {
    target_freq_kHz = source_freq_kHz;
  }


  int32_t i;

  //set Prescaler1
  int32_t pre1 = 2;
  if(target_freq_kHz <= APB1_CLOCK_MAX_FREQUECY_KHZ)
  {
    pre1 = 0;
  }
  else
  {
    for(i = 0; i < 4; i += 1)
    {
      if((target_freq_kHz / (pre1 << i)) <= APB1_CLOCK_MAX_FREQUECY_KHZ)
      {
        pre1 = i;
        break;
      }
    }
    pre1 |= (1 << 2);
  }
  
  int32_t pre2 = 2;
  if(target_freq_kHz <= APB2_CLOCK_MAX_FREQUECY_KHZ)
  {
    pre2 = 0;
  }
  else
  {
    for(i = 0; i < 4; i += 1)
    {
      if((target_freq_kHz / (pre2 << i)) <= APB2_CLOCK_MAX_FREQUECY_KHZ)
      {
        pre2 = i;
        break;
      }
    }
    pre2 |= (1 << 2);
  }

  cfgr &= ~((uint32_t)(0x3F << 10));
  cfgr |= ((uint32_t)((pre1 << 10) | (pre2 << 13)));

  RCC->CFGR = cfgr;
}

uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id)
{
  uint32_t clock_Hz = 0;

  switch(RCC->CFGR & RCC_CFGR_SWS)
  {
    case RCC_CFGR_SWS_HSI:
      clock_Hz = HSI_VALUE;
      break;

    case RCC_CFGR_SWS_HSE:
      clock_Hz = HSE_VALUE;
      break;

    case RCC_CFGR_SWS_PLL:
      {
        if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE)
        {
          clock_Hz = HSE_VALUE;
        }
        else
        {
          clock_Hz = HSI_VALUE;
        }

        uint32_t pll_m = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 0;
        uint32_t pll_n = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6;
        uint32_t pll_p = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16;

        pll_p = 2 + pll_p * 2;

        clock_Hz /= pll_m;
        clock_Hz *= pll_n;
        clock_Hz /= pll_p;
      }
      break;
  }

  switch(RCC->CFGR & RCC_CFGR_HPRE)
  {
    case RCC_CFGR_HPRE_DIV2:
      clock_Hz /= 2;
      break;

    case RCC_CFGR_HPRE_DIV4:
      clock_Hz /= 4;
      break;

    case RCC_CFGR_HPRE_DIV8:
      clock_Hz /= 8;
      break;

    case RCC_CFGR_HPRE_DIV16:
      clock_Hz /= 16;
      break;

    case RCC_CFGR_HPRE_DIV64:
      clock_Hz /= 64;
      break;

    case RCC_CFGR_HPRE_DIV128:
      clock_Hz /= 128;
      break;

    case RCC_CFGR_HPRE_DIV256:
      clock_Hz /= 256;
      break;

    case RCC_CFGR_HPRE_DIV512:
      clock_Hz /= 512;
      break;
  }

  if(clock_id == MCUAL_CLOCK_SYSTEM)
  {
    return clock_Hz;
  }

  if(clock_id == MCUAL_CLOCK_PERIPHERAL_1)
  {
    switch(RCC->CFGR & RCC_CFGR_PPRE1)
    {
      case RCC_CFGR_PPRE1_DIV2:
        clock_Hz /= 2;
        break;

      case RCC_CFGR_PPRE1_DIV4:
        clock_Hz /= 4;
        break;

      case RCC_CFGR_PPRE1_DIV8:
        clock_Hz /= 8;
        break;

      case RCC_CFGR_PPRE1_DIV16:
        clock_Hz /= 16;
        break;
    }

    return clock_Hz;
  }
  
  if(clock_id == MCUAL_CLOCK_PERIPHERAL_2)
  {
    switch(RCC->CFGR & RCC_CFGR_PPRE2)
    {
      case RCC_CFGR_PPRE2_DIV2:
        clock_Hz /= 2;
        break;

      case RCC_CFGR_PPRE2_DIV4:
        clock_Hz /= 4;
        break;

      case RCC_CFGR_PPRE2_DIV8:
        clock_Hz /= 8;
        break;

      case RCC_CFGR_PPRE2_DIV16:
        clock_Hz /= 16;
        break;
    }

    return clock_Hz;
  }

  return 0;
}

#endif
