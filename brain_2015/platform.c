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
  
  //init uart1 pin
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN7, 7);
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
