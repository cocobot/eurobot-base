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
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN9);
  
  //init gpio
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN8, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN8, MCUAL_GPIO_OUTPUT);
  
  //init encoders
  //enc rigth
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN0, 1);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN1, 1);

  //enc left
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN3, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN15, 2);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN3, 2);

  mcual_timer_init_encoder(MCUAL_TIMER2); 
  mcual_timer_init_encoder(MCUAL_TIMER5); 

  //init uart dbg pins
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_USART
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
  mcual_usart_init(PLATFORM_USART_DEBUG, 115200);
#endif

  //init adc
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN0, MCUAL_GPIO_INPUT);
#ifdef CONFIG_MCUAL_ADC
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN0, MCUAL_GPIO_FUNCTION_ANALOG);
  mcual_adc_init();
#endif

  //init spi
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN2, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
#ifdef CONFIG_MCUAL_SPI
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN10, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN11, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN12, 6);
#endif
  mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN2);

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

void platform_led_toggle(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN9);
  }
}

void platform_led_set(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN9);
  }
}

void platform_led_clear(uint8_t led)
{
  if(led & PLATFORM_LED0)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN9);
  }
}


void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction)
{
  (void)gpio;
  (void)direction;
}

void platform_gpio_set(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_EEPROM_WP)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN8);
  }
}

void platform_gpio_clear(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_EEPROM_WP)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN8);
  }
}

void platform_gpio_toggle(uint32_t gpio)
{
  if(gpio & PLATFORM_GPIO_EEPROM_WP)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN8);
  }
}

uint32_t platform_gpio_get(uint32_t gpio)
{
  uint32_t value = 0;

  if(gpio & PLATFORM_GPIO_STARTER)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN8) ? PLATFORM_GPIO_STARTER : 0;
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
  }

  return raw / 1000;
}


#ifdef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_MCUAL_SPI
void platform_spi_select(uint8_t select)
{
  //reset chip select
  mcual_gpio_set(MCUAL_GPIOD, (1 << 2));

  //clear the good one
  switch(select)
  {
    case PLATFORM_SPI_GYRO_SELECT:
      xSemaphoreTake(mutex_spi, portMAX_DELAY);
      mcual_gpio_clear(MCUAL_GPIOD, (1 << 2));
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
