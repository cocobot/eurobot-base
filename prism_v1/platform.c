#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 32000


void platform_init(void)
{
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_EXTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN6, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN1 | MCUAL_GPIO_PIN2 | MCUAL_GPIO_PIN3 | MCUAL_GPIO_PIN4 | MCUAL_GPIO_PIN5 | MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN0 | MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOH, MCUAL_GPIO_PIN0 | MCUAL_GPIO_PIN1 | MCUAL_GPIO_PIN5 | MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_OUTPUT);
  platform_led_clear(0xffffffff);
}

void platform_led_toggle(uint32_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN3);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED3)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED4)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN6);
  }

  if(led & PLATFORM_LED5)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN7);
  }
  
  if(led & PLATFORM_LED_ERROR)
  {
    mcual_gpio_toogle(MCUAL_GPIOH, MCUAL_GPIO_PIN5);
  }
  
  if(led & PLATFORM_LED_RUN)
  {
    mcual_gpio_toogle(MCUAL_GPIOH, MCUAL_GPIO_PIN6);
  }
  
  if(led & PLATFORM_LED_COM)
  {
    mcual_gpio_toogle(MCUAL_GPIOH, MCUAL_GPIO_PIN7);
  }

  if(led & PLATFORM_LED_SYNC0)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_SYNC1)
  {
    mcual_gpio_toogle(MCUAL_GPIOH, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_LOWBAT)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX0)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX1)
  {
    mcual_gpio_toogle(MCUAL_GPIOH, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_RF)
  {
    mcual_gpio_toogle(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN3);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED3)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED4)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN6);
  }

  if(led & PLATFORM_LED5)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN7);
  }
  
  if(led & PLATFORM_LED_ERROR)
  {
    mcual_gpio_set(MCUAL_GPIOH, MCUAL_GPIO_PIN5);
  }
  
  if(led & PLATFORM_LED_RUN)
  {
    mcual_gpio_set(MCUAL_GPIOH, MCUAL_GPIO_PIN6);
  }
  
  if(led & PLATFORM_LED_COM)
  {
    mcual_gpio_set(MCUAL_GPIOH, MCUAL_GPIO_PIN7);
  }

  if(led & PLATFORM_LED_SYNC0)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_SYNC1)
  {
    mcual_gpio_set(MCUAL_GPIOH, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_LOWBAT)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX0)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX1)
  {
    mcual_gpio_set(MCUAL_GPIOH, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_RF)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
}


void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN3);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED3)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED4)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN6);
  }

  if(led & PLATFORM_LED5)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN7);
  }
  
  if(led & PLATFORM_LED_ERROR)
  {
    mcual_gpio_clear(MCUAL_GPIOH, MCUAL_GPIO_PIN5);
  }
  
  if(led & PLATFORM_LED_RUN)
  {
    mcual_gpio_clear(MCUAL_GPIOH, MCUAL_GPIO_PIN6);
  }
  
  if(led & PLATFORM_LED_COM)
  {
    mcual_gpio_clear(MCUAL_GPIOH, MCUAL_GPIO_PIN7);
  }

  if(led & PLATFORM_LED_SYNC0)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_SYNC1)
  {
    mcual_gpio_clear(MCUAL_GPIOH, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_LOWBAT)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX0)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }

  if(led & PLATFORM_LED_IDX1)
  {
    mcual_gpio_clear(MCUAL_GPIOH, MCUAL_GPIO_PIN0);
  }

  if(led & PLATFORM_LED_RF)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
}
