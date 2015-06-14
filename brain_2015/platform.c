#include <stm32f4xx.h>
#include "platform.h"

#define PLATFORM_PLLM 8
#define PLATFORM_PLLN 336
#define PLATFORM_PLLP 2
#define PLATFORM_PLLQ 7


void platform_init(void)
{
  //init Quartz and PLL
  RCC_DeInit();

  RCC_HSEConfig(RCC_HSE_ON);
  RCC_WaitForHSEStartUp();

  RCC_PLLConfig(RCC_PLLSource_HSE, PLATFORM_PLLM, PLATFORM_PLLN, PLATFORM_PLLP, PLATFORM_PLLQ);

  RCC_PLLCmd(ENABLE);

  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  
  //init leds
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  GPIO_InitTypeDef GPIOInitStruct_G;
  GPIOInitStruct_G.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIOInitStruct_G.GPIO_Speed = GPIO_Speed_50MHz;
  GPIOInitStruct_G.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOG, &GPIOInitStruct_G);
}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
  }

  if(led & PLATFORM_LED1)
  {
    GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
  }
}
