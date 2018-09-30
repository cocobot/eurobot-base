#include "generated/autoconf.h"
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <semphr.h>
# include <task.h>
#endif
#include "platform.h"
#include <stm32f4xx.h> //todo remove me

#define PLATFORM_MAIN_CLOCK_KHZ 168000

#ifdef CONFIG_OS_USE_FREERTOS
//mutexes for spi access
static SemaphoreHandle_t mutex_i2c;
#endif


void platform_init(void)
{
  //init mutexes
#ifdef CONFIG_OS_USE_FREERTOS
  mutex_i2c = xSemaphoreCreateMutex();
#endif

  //init clock
    mcual_clock_init(MCUAL_CLOCK_SOURCE_EXTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
    //init leds
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN13, MCUAL_GPIO_OUTPUT);
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN14, MCUAL_GPIO_OUTPUT);
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN14);
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN15);

    //init gpio
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);

#ifdef CONFIG_MCUAL_I2C
  //init i2c
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN6, 4);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN7, 4);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN6, MCUAL_GPIO_OPEN_DRAIN);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN7, MCUAL_GPIO_OPEN_DRAIN);
  //ugly internal pull up for i2c...
  GPIOB->PUPDR |= 0x01 << 12;
  GPIOB->PUPDR |= 0x01 << 14;
  mcual_i2c_master_init(MCUAL_I2C1, 100000);
#endif
//  //init uart dbg pins
//  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
//  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
//  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
//  mcual_usart_init(PLATFORM_USART_DEBUG, 115200);
//
}

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED_GREEN)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }

  if(led & PLATFORM_LED_ORANGE)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED_RED)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN14);
  }

  if(led & PLATFORM_LED_BLUE)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN15);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED_GREEN)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }

  if(led & PLATFORM_LED_ORANGE)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED_RED)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN14);
  }

  if(led & PLATFORM_LED_BLUE)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN15);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED_GREEN)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }

  if(led & PLATFORM_LED_ORANGE)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }

  if(led & PLATFORM_LED_RED)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN14);
  }

  if(led & PLATFORM_LED_BLUE)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN15);
  }
}

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
   if(gpio & PLATFORM_BUTTON_USER)
   {
       mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, direction);
   }
}

void platform_gpio_set(uint32_t gpio)
{
   if(gpio & PLATFORM_BUTTON_USER)
   {
       mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
   }
}

void platform_gpio_clear(uint32_t gpio)
{
   if(gpio & PLATFORM_BUTTON_USER)
   {
       mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
   }
}

void platform_gpio_toggle(uint32_t gpio)
{
   if(gpio & PLATFORM_BUTTON_USER)
   {
     mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
   }
}

uint32_t platform_gpio_get(uint32_t gpio)
{ 
    uint32_t value = 0;

    if(gpio & PLATFORM_BUTTON_USER)
    {
        value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN0) ? PLATFORM_BUTTON_USER : 0;
    }

    return value;
}

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
