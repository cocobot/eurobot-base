#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 168000


void platform_init(void)
{
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_EXTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN4, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN5, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  
  //init gpio
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN8, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN9, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN12, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN13, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN14, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN15, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN13, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN14, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN15, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN1, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN2, MCUAL_GPIO_INPUT);

  //init uart dbg pins
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
  mcual_usart_init(PLATFORM_USART_DEBUG, 115200);

  //init adc
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN1, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN2, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN1, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN2, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_adc_init();
}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }

  if(led & PLATFORM_LED1)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
  }

  if(led & PLATFORM_LED2)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
}


void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN7, direction);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN8, direction);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN9, direction);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN10, direction);
  }
  if(gpio & PLATFORM_GPIO4)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN11, direction);
  }
  if(gpio & PLATFORM_GPIO5)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN12, direction);
  }
    if(gpio & PLATFORM_GPIO6)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN13, direction);
  }
  if(gpio & PLATFORM_GPIO7)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN14, direction);
  }
  if(gpio & PLATFORM_GPIO8)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN15, direction);
  }
  if(gpio & PLATFORM_GPIO9)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN13, direction);
  }
  if(gpio & PLATFORM_GPIO10)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN14, direction);
  }
  if(gpio & PLATFORM_GPIO11)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN15, direction);
  }
  if(gpio & PLATFORM_GPIO_STARTER)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN1, direction);
  }
  if(gpio & PLATFORM_GPIO_COLOR)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN2, direction);
  }
}

void platform_gpio_set(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO4)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO5)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
    if(gpio & PLATFORM_GPIO6)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO7)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO8)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO9)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO10)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO11)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_STARTER)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_GPIO_COLOR)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
}

void platform_gpio_clear(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO4)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO5)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
    if(gpio & PLATFORM_GPIO6)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO7)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO8)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO9)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO10)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO11)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_STARTER)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_GPIO_COLOR)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
}

void platform_gpio_toogle(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO0)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO1)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO2)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO3)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO4)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO5)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
    if(gpio & PLATFORM_GPIO6)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO7)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO8)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO9)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO10)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO11)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_STARTER)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_GPIO_COLOR)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  if(gpio & PLATFORM_GPIO0)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN7) ? PLATFORM_GPIO0 : 0;
  }
  if(gpio & PLATFORM_GPIO1)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN8) ? PLATFORM_GPIO1 : 0;
  }
  if(gpio & PLATFORM_GPIO2)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN9) ? PLATFORM_GPIO2 : 0;
  }
  if(gpio & PLATFORM_GPIO3)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN10) ? PLATFORM_GPIO3 : 0;
  }
  if(gpio & PLATFORM_GPIO4)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN11) ? PLATFORM_GPIO4 : 0;
  }
  if(gpio & PLATFORM_GPIO5)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN12) ? PLATFORM_GPIO5 : 0;
  }
  if(gpio & PLATFORM_GPIO6)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN13) ? PLATFORM_GPIO6 : 0;
  }
  if(gpio & PLATFORM_GPIO7)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN14) ? PLATFORM_GPIO7 : 0;
  }
  if(gpio & PLATFORM_GPIO8)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN15) ? PLATFORM_GPIO8 : 0;
  }
  if(gpio & PLATFORM_GPIO9)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN13) ? PLATFORM_GPIO9 : 0;
  }
  if(gpio & PLATFORM_GPIO10)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN14) ? PLATFORM_GPIO10 : 0;
  }
  if(gpio & PLATFORM_GPIO11)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN15) ? PLATFORM_GPIO11 : 0;
  }
  if(gpio & PLATFORM_GPIO_STARTER)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN1) ? PLATFORM_GPIO_STARTER : 0;
  }
  if(gpio & PLATFORM_GPIO_COLOR)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN2) ? PLATFORM_GPIO_COLOR : 0;
  }

  return value;
}

int32_t platform_adc_get_mV(uint32_t adc)
{
  int32_t raw = mcual_adc_get(adc);

  raw *= 3300000 / 4096;

  switch(adc)
  {
    case PLATFORM_ADC_VBAT:
      raw *= 11;
      break;

    case PLATFORM_ADC_IR0:
    case PLATFORM_ADC_IR1:
      raw *= 2;
      break;
  }

  return raw / 1000;
}
