#include <mcual.h>
#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 180000


void platform_init(void)
{
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_EXTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 

  
  //init leds
  mcual_gpio_init(MCUAL_GPIOG, MCUAL_GPIO_PIN13, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOG, MCUAL_GPIO_PIN14, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOG, MCUAL_GPIO_PIN13);
  mcual_gpio_clear(MCUAL_GPIOG, MCUAL_GPIO_PIN14);
  /*
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  GPIO_InitTypeDef GPIOInitStruct_G;
  GPIOInitStruct_G.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIOInitStruct_G.GPIO_Speed = GPIO_Speed_50MHz;
  GPIOInitStruct_G.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOG, &GPIOInitStruct_G);
  */
}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toogle(MCUAL_GPIOG, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_toogle(MCUAL_GPIOG, MCUAL_GPIO_PIN14);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOG, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_set(MCUAL_GPIOG, MCUAL_GPIO_PIN14);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOG, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_clear(MCUAL_GPIOG, MCUAL_GPIO_PIN14);
  }
}
