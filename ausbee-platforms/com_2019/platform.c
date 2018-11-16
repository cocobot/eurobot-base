#include "generated/autoconf.h"
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <semphr.h>
# include <task.h>
#endif
#include "platform.h"

#define PLATFORM_MAIN_CLOCK_KHZ 168000

#ifdef CONFIG_OS_USE_FREERTOS
//mutexes for spi access
#ifdef CONFIG_MCUAL_SPI
static SemaphoreHandle_t mutex_spi;
#endif
#ifdef CONFIG_MCUAL_I2C
static SemaphoreHandle_t mutex_i2c;
#endif
#endif

void platform_init(void)
{
  //init mutexes
#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_SPI
  mutex_spi = xSemaphoreCreateMutex();
#endif
#ifdef CONFIG_MCUAL_I2C
  mutex_i2c = xSemaphoreCreateMutex();
#endif
#endif

  //init clock
  mcual_clock_init(MCUAL_CLOCK_SOURCE_INTERNAL, PLATFORM_MAIN_CLOCK_KHZ); 
  
  //init leds
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15);
  mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13);
  mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN11 | MCUAL_GPIO_PIN12 | MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15);
  
  //init gpio
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0 | MCUAL_GPIO_PIN1 | MCUAL_GPIO_PIN2 | MCUAL_GPIO_PIN3 | MCUAL_GPIO_PIN4 | MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN4 | MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7 | MCUAL_GPIO_PIN9, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN14 | MCUAL_GPIO_PIN15, MCUAL_GPIO_INPUT);

  //init uart dbg pins
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_USART
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
  mcual_usart_init(PLATFORM_USART_DEBUG, 115200);
#endif
  
#ifdef PLATFORM_BRAIN_2019_USB
  //init uart user pins
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_USART
  mcual_gpio_set_function(MCUAL_GPIOD, MCUAL_GPIO_PIN5, 7);
  mcual_gpio_set_function(MCUAL_GPIOD, MCUAL_GPIO_PIN6, 7);
  mcual_usart_init(PLATFORM_USART_USER, 115200);
#endif
#endif


  //init spi
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN8, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
#ifdef CONFIG_MCUAL_SPI
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN10, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN11, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN12, 6);
#endif
  mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN8);

#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_SPI
  platform_spi_select(PLATFORM_SPI_CS_UNSELECT);
  mcual_spi_master_init(MCUAL_SPI3, MCUAL_SPI_MODE_3, 400000);
#endif
#endif

  //init i2c
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_I2C
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN6, 4);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN7, 4);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN6, MCUAL_GPIO_OPEN_DRAIN);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN7, MCUAL_GPIO_OPEN_DRAIN);
  mcual_i2c_master_init(MCUAL_I2C1, 100000);
#endif
}

void platform_led_toggle(uint32_t led)
{
  if(led & PLATFORM_LED_GREEN_0)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_1)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_GREEN_2)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_GREEN_3)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_GREEN_4)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_GREEN_5)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_GREEN_6)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_GREEN_7)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_GREEN_8)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_9)
  {
    mcual_gpio_toggle(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_0)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_1)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_RED_2)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_RED_3)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_RED_4)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_RED_5)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_6)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_RED_7)
  {
    mcual_gpio_toggle(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_RED_8)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_9)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
}

void platform_led_set(uint32_t led)
{
  if(led & PLATFORM_LED_GREEN_0)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_1)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_GREEN_2)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_GREEN_3)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_GREEN_4)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_GREEN_5)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_GREEN_6)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_GREEN_7)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_GREEN_8)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_9)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_0)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_1)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_RED_2)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_RED_3)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_RED_4)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_RED_5)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_6)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_RED_7)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_RED_8)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_9)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
}

void platform_led_clear(uint32_t led)
{
  if(led & PLATFORM_LED_GREEN_0)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_1)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_GREEN_2)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_GREEN_3)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_GREEN_4)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_GREEN_5)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_GREEN_6)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_GREEN_7)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_GREEN_8)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_GREEN_9)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_0)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_1)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN9);
  }
  if(led & PLATFORM_LED_RED_2)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN11);
  }
  if(led & PLATFORM_LED_RED_3)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN8);
  }
  if(led & PLATFORM_LED_RED_4)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN12);
  }
  if(led & PLATFORM_LED_RED_5)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN13);
  }
  if(led & PLATFORM_LED_RED_6)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN14);
  }
  if(led & PLATFORM_LED_RED_7)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN15);
  }
  if(led & PLATFORM_LED_RED_8)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN10);
  }
  if(led & PLATFORM_LED_RED_9)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN11);
  }
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


#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_SPI
void platform_spi_select(uint8_t select)
{
  //reset chip select
  mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN8);

  //clear the good one
  switch(select)
  {
    case PLATFORM_SPI_RF_SELECT:
      xSemaphoreTake(mutex_spi, portMAX_DELAY);
      mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
      break;

    default:
      xSemaphoreGive(mutex_spi);
      break;
  }
}
#endif
#endif

#ifdef CONFIG_MCUAL_SPI
uint8_t platform_spi_transfert(uint8_t data)
{
  return mcual_spi_master_transfert(MCUAL_SPI3, data);
}
#endif

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
