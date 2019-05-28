#include "generated/autoconf.h"
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <semphr.h>
# include <task.h>
#endif
#include <stdio.h>
#include <cocobot.h>
#include "platform.h"
#include "pcm9685.h"

#define PLATFORM_MAIN_CLOCK_KHZ 168000

#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_I2C
static SemaphoreHandle_t mutex_i2c;
#endif
#endif

#include <stdarg.h>
char buf[512];
void uprintf(char * fmt, ...)
{
   va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  
  uint8_t * ptr = (uint8_t *)buf;
  while(*ptr) {
    mcual_usart_send(PLATFORM_USART_DEBUG, *ptr);
    ptr += 1;
  }
}

void platform_init(void)
{
  //init mutexes
#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_I2C
  mutex_i2c = xSemaphoreCreateMutex();
#endif
#endif
  
  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_INTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN2, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  
  //init gpio
  mcual_gpio_init(MCUAL_GPIOB, 
                  MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN15,
                  MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOE, 
                  MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15,
                  MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOD, 
                  MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11,
                  MCUAL_GPIO_OUTPUT);
  platform_gpio_clear(0xFFFFFFFF);
  
  //init uart dbg pins
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_USART
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
  mcual_usart_init(PLATFORM_USART_DEBUG, 921600);
#endif
  
  //init adc
  mcual_gpio_init(MCUAL_GPIOA, 
                  MCUAL_GPIO_PIN4 | MCUAL_GPIO_PIN5 | MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7,
                  MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_ADC
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN4, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN5, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN6, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN7, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_adc_init();
#endif

  //init can pins
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOD, MCUAL_GPIO_PIN0, 9);
  mcual_gpio_set_function(MCUAL_GPIOD, MCUAL_GPIO_PIN1, 9);

  //init i2c
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_I2C
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN6, MCUAL_GPIO_OPEN_DRAIN);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN7, MCUAL_GPIO_OPEN_DRAIN);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN6, 4);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN7, 4);
  mcual_i2c_master_init(MCUAL_I2C1, 100000);
#endif

}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
}


void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
  (void)gpio;
  (void)direction;
}

void platform_gpio_set(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_PUMP1)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO_PUMP2)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE1)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE2)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE3)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE4)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(gpio & PLATFORM_GPIO_VALVE5)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO_VALVE6)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE7)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE8)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
}

void platform_gpio_clear(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_PUMP1)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO_PUMP2)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE1)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE2)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE3)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE4)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(gpio & PLATFORM_GPIO_VALVE5)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO_VALVE6)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE7)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE8)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
}

void platform_gpio_toggle(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_PUMP1)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(gpio & PLATFORM_GPIO_PUMP2)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE1)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE2)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE3)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(gpio & PLATFORM_GPIO_VALVE4)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(gpio & PLATFORM_GPIO_VALVE5)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(gpio & PLATFORM_GPIO_VALVE6)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(gpio & PLATFORM_GPIO_VALVE7)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(gpio & PLATFORM_GPIO_VALVE8)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  (void)gpio;

  return value;
}

int32_t platform_adc_get_mV(uint32_t adc)
{
  int32_t raw = mcual_adc_get(adc);

  raw *= 3300000 / 4096;

  return raw / 1000;
}


#ifdef CONFIG_MCUAL_I2C
uint8_t platform_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
{
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreTake(mutex_i2c, portMAX_DELAY);
#endif
  uint8_t ret = mcual_i2c_transmit(id, addr, txbuf, tx_size, rxbuf, rx_size);
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreGive(mutex_i2c);
#endif

  return ret;
}
#endif

void platform_servo_set_value(uint32_t servo_id, uint32_t value)
{
//#ifdef AUSBEE_SIM
  cocobot_com_printf(COM_DEBUG, "platform_servo_set_value %lu %lu\n", servo_id, value);
//#endif
  pcm9685_set_channel(servo_id, 0, value);
}
