#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CLOCK

#include <stm32l4xx.h>
#include <stm32l4xx_hal.h>
#include <mcual.h>

extern int main(void);
extern unsigned int _sdata;
extern unsigned int _sidata;
extern unsigned int _edata;
extern unsigned int _sbss;
extern unsigned int _ebss;

void Reset_Handler(void)
{
  unsigned int *src, *dst;

  /* Copy data section from flash to RAM */
  src = &_sidata;
  dst = &_sdata;
  while (dst < &_edata)
    *dst++ = *src++;

  /* Clear the bss section */
  dst = &_sbss;
  while (dst < &_ebss)
    *dst++ = 0;

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
  RCC->CR |= RCC_CR_HSION;
  RCC->CFGR = 0x00000000;
  //RCC->CFGR |= RCC_CFGR_MCO2PRE;
  RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON | RCC_CR_HSEBYP);
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
  PWR->CR1 |= PWR_CR1_VOS;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV16;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV16;

  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
  SCB->VTOR = FLASH_START;

  main();
}

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  (void)source;
  (void)target_freq_kHz;

  HAL_Init();

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 16;

  //4 Mhz as input
  RCC_OscInitStruct.PLL.PLLM = 16/4;

  // 4 * target as output
  RCC_OscInitStruct.PLL.PLLN = target_freq_kHz * 4 / 1000;
  RCC_OscInitStruct.PLL.PLLP = 4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = RCC_OscInitStruct.PLL.PLLN;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id)
{
  (void)clock_id;
  return 0;
}

#endif
