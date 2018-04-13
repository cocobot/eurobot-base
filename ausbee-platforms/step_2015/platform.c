#include "generated/autoconf.h"
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <semphr.h>
# include <task.h>
#endif
#include "platform.h"


void platform_init(void)
{
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_INTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN3, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN3);
  
  //init gpio
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN4, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN5, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN6, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN3);
  }
}


void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN3);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN3);
  }
}


void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN4, direction);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN5, direction);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN6, direction);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN7, direction);
  }
}

void platform_gpio_set(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN7);
  }
}

void platform_gpio_clear(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN7);
  }
}

void platform_gpio_toggle(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN7);
  }
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  if(gpio & PLATFORM_GPIO0)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN4) ? PLATFORM_GPIO0 : 0;
  }
  if(gpio & PLATFORM_GPIO1)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN5) ? PLATFORM_GPIO1 : 0;
  }
  if(gpio & PLATFORM_GPIO2)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN6) ? PLATFORM_GPIO2 : 0;
  }
  if(gpio & PLATFORM_GPIO3)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN7) ? PLATFORM_GPIO3 : 0;
  }

  return value;
}
