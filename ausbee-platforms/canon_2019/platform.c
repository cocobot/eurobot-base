#include <stdio.h>
#include <stdarg.h>
#include "generated/autoconf.h"
#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 80000


char buf[512];
void uprintf(char * fmt, ...)
{
   va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  
  uint8_t * ptr = (uint8_t *)buf;
  while(*ptr) {
    mcual_usart_send(PLATFORM_USART_USER, *ptr);
    ptr += 1;
  }
}

void platform_init(void)
{
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_INTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN14, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  
  //init outputs
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN6, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN7, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN8, MCUAL_GPIO_OUTPUT);
  platform_gpio_clear(PLATFORM_GPIO_UEN | PLATFORM_GPIO_VEN | PLATFORM_GPIO_WEN);
  
  //init inputs
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN1, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN2, MCUAL_GPIO_INPUT);

  //init motor outputs
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN8, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN8, 1);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 1);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 1);
  platform_set_frequency(PLATFORM_PWM_U | PLATFORM_PWM_V | PLATFORM_PWM_W, 1000);
  platform_set_duty_cycle(PLATFORM_PWM_U, 0);
  platform_set_duty_cycle(PLATFORM_PWM_V, 0);
  platform_set_duty_cycle(PLATFORM_PWM_W, 0);
  
  //init uart pins
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN10, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN10, 7);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN11, 7);
  mcual_usart_init(PLATFORM_USART_USER, 115200);

}

void platform_led_toggle(uint32_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
}

void platform_led_set(uint32_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
}

void platform_led_clear(uint32_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
}

void platform_gpio_set(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_UEN)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO_VEN)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_WEN)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
}

void platform_gpio_clear(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_UEN)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO_VEN)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_WEN)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
}

void platform_gpio_toggle(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_UEN)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_GPIO_VEN)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_WEN)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  if(gpio & PLATFORM_GPIO_UHALL)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN0) ? PLATFORM_GPIO_UHALL : 0;
  }
  if(gpio & PLATFORM_GPIO_VHALL)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN1) ? PLATFORM_GPIO_VHALL : 0;
  }
  if(gpio & PLATFORM_GPIO_WHALL)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN2) ? PLATFORM_GPIO_WHALL : 0;
  }

  return value;
}

void platform_gpio_set_interrupt(uint32_t gpio, mcual_gpio_edge_t edge, mcual_gpio_interrupt_handler_t handler)
{
  if(gpio & PLATFORM_GPIO_UHALL)
  {
    mcual_gpio_set_interrupt(MCUAL_GPIOA, MCUAL_GPIO_PIN1, edge, handler);
  }
  if(gpio & PLATFORM_GPIO_VHALL)
  {
    mcual_gpio_set_interrupt(MCUAL_GPIOA, MCUAL_GPIO_PIN0, edge, handler);
  }
  if(gpio & PLATFORM_GPIO_WHALL)
  {
    mcual_gpio_set_interrupt(MCUAL_GPIOA, MCUAL_GPIO_PIN2, edge, handler);
  }
}

void platform_set_frequency(uint32_t pwm_output, uint32_t freq_Hz)
{
  if(pwm_output & (PLATFORM_PWM_U | PLATFORM_PWM_V | PLATFORM_PWM_W))
  {
    (void)freq_Hz;
    mcual_timer_init(MCUAL_TIMER1, freq_Hz);
    mcual_timer_enable_channel(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL1);
    mcual_timer_enable_channel(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL2);
    mcual_timer_enable_channel(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL3);
  }
}

void platform_set_duty_cycle(uint32_t pwm_output, uint32_t duty_cycle)
{
  if(pwm_output & PLATFORM_PWM_U)
  {
    mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL1, duty_cycle);
  }
  if(pwm_output & PLATFORM_PWM_V)
  {
    mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL2, duty_cycle);
  }
  if(pwm_output & PLATFORM_PWM_W)
  {
    mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL3, duty_cycle);
  }
}
