#include "generated/autoconf.h"
#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 48000

void platform_init(void)
{

  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_INTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init uart dbg pins
#ifdef CONFIG_MCUAL_USART
#ifdef CONFIG_PLATFORM_CANIVO_2019_UART
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN2, 7);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN3œ, 7);
  mcual_usart_init(PLATFORM_USART_USER, 115200);
#endif
#endif
}

void platform_led_toggle(uint32_t led)
{
  (void)led;
}

void platform_led_set(uint32_t led)
{
  (void)led;
}

void platform_led_clear(uint32_t led)
{
  (void)led;
}

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
  (void)gpio;
  (void)direction;
}

void platform_gpio_set(uint32_t gpio)
{
  (void)gpio;
}

void platform_gpio_clear(uint32_t gpio)
{
  (void)gpio;
}

void platform_gpio_toggle(uint32_t gpio)
{
  (void)gpio; 
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  (void)gpio;

  return value;
}
