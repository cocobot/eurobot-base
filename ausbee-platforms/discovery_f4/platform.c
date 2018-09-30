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
static SemaphoreHandle_t mutex_spi_slave;
static SemaphoreHandle_t mutex_spi_position;
static SemaphoreHandle_t mutex_i2c;
#endif


void platform_init(void)
{
  //init mutexes
#ifdef CONFIG_OS_USE_FREERTOS
  mutex_spi_position = xSemaphoreCreateMutex();
  mutex_spi_slave = xSemaphoreCreateMutex();
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


//  //init uart dbg pins
//  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN9, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN10, MCUAL_GPIO_INPUT);
//  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN9, 7);
//  mcual_gpio_set_function(MCUAL_GPIOA, MCUAL_GPIO_PIN10, 7);
//  mcual_usart_init(PLATFORM_USART_DEBUG, 115200);
//
//  //init spi
//  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN0 | MCUAL_GPIO_PIN1 | MCUAL_GPIO_PIN2 | MCUAL_GPIO_PIN3, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
//  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN10, 6);
//  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN11, 6);
//  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN12, 6);
//#ifdef CONFIG_OS_USE_FREERTOS
//  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
//  mcual_spi_master_init(MCUAL_SPI3, MCUAL_SPI_MODE_3, 400000);
//#endif
//
//  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN14, MCUAL_GPIO_INPUT);
//  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
//  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN13, 5);
//  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN14, 5);
//  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN15, 5);
//#ifdef CONFIG_OS_USE_FREERTOS
//  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
//  mcual_spi_master_init(MCUAL_SPI2, MCUAL_SPI_MODE_3, 400000);
//#endif
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

#ifdef CONFIG_OS_USE_FREERTOS
void platform_spi_slave_select(uint8_t select)
{
//  if(select != PLATFORM_SPI_CS_UNSELECT)
//  {
//    xSemaphoreTake(mutex_spi_slave, portMAX_DELAY);
//  }
//
//  mcual_gpio_set(MCUAL_GPIOD, select & 0x0F); 
//  mcual_gpio_clear(MCUAL_GPIOD, (~select) & 0x0F); 
//
//  if(select == PLATFORM_SPI_CS_UNSELECT)
//  {
//    xSemaphoreGive(mutex_spi_slave);
//  }
}
#endif
//
uint8_t platform_spi_slave_transfert(uint8_t data)
{
//  return mcual_spi_master_transfert(MCUAL_SPI3, data)/}
}
//
#ifdef CONFIG_OS_USE_FREERTOS
void platform_spi_position_select(uint8_t select)
{
//  //reset chip select
//  mcual_gpio_set(MCUAL_GPIOD, (1 << 8) | (1 << 9) | (1 << 10));
//
//  //clear the good one
//  switch(select)
//  {
//    case PLATFORM_SPI_ENCR_SELECT:
//      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
//      mcual_gpio_clear(MCUAL_GPIOD, (1 << 9));
//      break;
//
//    case PLATFORM_SPI_ENCL_SELECT:
//      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
//      mcual_gpio_clear(MCUAL_GPIOD, (1 << 8));
//      break;
//
//    case PLATFORM_SPI_ENCG_SELECT:
//      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
//      mcual_gpio_clear(MCUAL_GPIOD, (1 << 10));
//      break;
//
//    default:
//      xSemaphoreGive(mutex_spi_position);
//      break;
//  }
}
#endif
//
uint8_t platform_spi_position_transfert(uint8_t data)
{
//  return mcual_spi_master_transfert(MCUAL_SPI2, data);
}
//
#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_frequency(uint32_t freq_Hz)
{
//  mcual_timer_init(MCUAL_TIMER9, freq_Hz);
}
#endif
//
#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_left_duty_cycle(uint32_t duty_cycle)
{
//  mcual_timer_set_duty_cycle(MCUAL_TIMER9, MCUAL_TIMER_CHANNEL1, duty_cycle);
}
#endif
//
#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_right_duty_cycle(uint32_t duty_cycle)
{
//  mcual_timer_set_duty_cycle(MCUAL_TIMER9, MCUAL_TIMER_CHANNEL2, duty_cycle);
}
#endif

