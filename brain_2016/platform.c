#include "generated/autoconf.h"
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <semphr.h>
#endif
#include "platform.h"
#include "pcm9685.h"

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

  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN4, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN8, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN9, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN2, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN3, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN4, MCUAL_GPIO_INPUT);
  
  //init motor pwm pins
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN5, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN6, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOE, MCUAL_GPIO_PIN5, 3);
  mcual_gpio_set_function(MCUAL_GPIOE, MCUAL_GPIO_PIN6, 3);
  //set motors default frequency and duty cycle
  platform_gpio_set_direction(PLATFORM_GPIO_MOTOR_DIR_RIGHT | PLATFORM_GPIO_MOTOR_DIR_LEFT | PLATFORM_GPIO_MOTOR_ENABLE, MCUAL_GPIO_OUTPUT);
  platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_RIGHT | PLATFORM_GPIO_MOTOR_DIR_LEFT | PLATFORM_GPIO_MOTOR_ENABLE);
#ifdef CONFIG_MCUAL_TIMER
  platform_motor_set_frequency(20000);
  platform_motor_set_left_duty_cycle(0x0000);
  platform_motor_set_right_duty_cycle(0x0000);
  mcual_timer_enable_channel(MCUAL_TIMER9, MCUAL_TIMER_CHANNEL1 | MCUAL_TIMER_CHANNEL2);
#endif


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
#ifdef CONFIG_MCUAL_ADC
  mcual_adc_init();
#endif

  //init spi
  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN0 | MCUAL_GPIO_PIN1 | MCUAL_GPIO_PIN2 | MCUAL_GPIO_PIN3, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN11, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN10 | MCUAL_GPIO_PIN12, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN10, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN11, 6);
  mcual_gpio_set_function(MCUAL_GPIOC, MCUAL_GPIO_PIN12, 6);
#ifdef CONFIG_OS_USE_FREERTOS
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
  mcual_spi_master_init(MCUAL_SPI3, MCUAL_SPI_MODE_3, 400000);
#endif

  mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN8 | MCUAL_GPIO_PIN9 | MCUAL_GPIO_PIN10, MCUAL_GPIO_OUTPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN14, MCUAL_GPIO_INPUT);
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN13 | MCUAL_GPIO_PIN15, MCUAL_GPIO_OUTPUT);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN13, 5);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN14, 5);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN15, 5);
#ifdef CONFIG_OS_USE_FREERTOS
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
  mcual_spi_master_init(MCUAL_SPI2, MCUAL_SPI_MODE_3, 400000);
#endif

  //init i2c
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN6 | MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN6, 4);
  mcual_gpio_set_function(MCUAL_GPIOB, MCUAL_GPIO_PIN7, 4);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN6, MCUAL_GPIO_OPEN_DRAIN);
  mcual_gpio_set_output_type(MCUAL_GPIOB, MCUAL_GPIO_PIN7, MCUAL_GPIO_OPEN_DRAIN);
  mcual_i2c_master_init(MCUAL_I2C1, 100000);
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
  if(gpio & PLATFORM_GPIO_ALARM0)
  {
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN4, direction);
  }
  if(gpio & PLATFORM_GPIO_ALARM1)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN8, direction);
  }
  if(gpio & PLATFORM_GPIO_ALARM2)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN9, direction);
  }
  if(gpio & PLATFORM_GPIO_ALARM3)
  {
    mcual_gpio_init(MCUAL_GPIOD, MCUAL_GPIO_PIN7, direction);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_RIGHT)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN4, direction);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_LEFT)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN3, direction);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_ENABLE)
  {
    mcual_gpio_init(MCUAL_GPIOE, MCUAL_GPIO_PIN2, direction);
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
  if(gpio & PLATFORM_GPIO_ALARM0)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_ALARM1)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO_ALARM2)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO_ALARM3)
  {
    mcual_gpio_set(MCUAL_GPIOD, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_RIGHT)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_LEFT)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_ENABLE)
  {
    mcual_gpio_set(MCUAL_GPIOE, MCUAL_GPIO_PIN2);
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
  if(gpio & PLATFORM_GPIO_ALARM0)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_ALARM1)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO_ALARM2)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO_ALARM3)
  {
    mcual_gpio_clear(MCUAL_GPIOD, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_RIGHT)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_LEFT)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_ENABLE)
  {
    mcual_gpio_clear(MCUAL_GPIOE, MCUAL_GPIO_PIN2);
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
  if(gpio & PLATFORM_GPIO_ALARM0)
  {
    mcual_gpio_toogle(MCUAL_GPIOD, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_ALARM1)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN8);
  }
  if(gpio & PLATFORM_GPIO_ALARM2)
  {
    mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN9);
  }
  if(gpio & PLATFORM_GPIO_ALARM3)
  {
    mcual_gpio_toogle(MCUAL_GPIOD, MCUAL_GPIO_PIN7);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_RIGHT)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_LEFT)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_GPIO_MOTOR_ENABLE)
  {
    mcual_gpio_toogle(MCUAL_GPIOE, MCUAL_GPIO_PIN2);
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
  if(gpio & PLATFORM_GPIO_ALARM0)
  {
    value |= mcual_gpio_get(MCUAL_GPIOD, MCUAL_GPIO_PIN4) ? PLATFORM_GPIO_ALARM0 : 0;
  }
  if(gpio & PLATFORM_GPIO_ALARM1)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN8) ? PLATFORM_GPIO_ALARM1 : 0;
  }
  if(gpio & PLATFORM_GPIO_ALARM2)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN9) ? PLATFORM_GPIO_ALARM2 : 0;
  }
  if(gpio & PLATFORM_GPIO_ALARM3)
  {
    value |= mcual_gpio_get(MCUAL_GPIOD, MCUAL_GPIO_PIN7) ? PLATFORM_GPIO_ALARM3 : 0;
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_RIGHT)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN4) ? PLATFORM_GPIO_MOTOR_DIR_RIGHT : 0;
  }
  if(gpio & PLATFORM_GPIO_MOTOR_DIR_LEFT)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN3) ? PLATFORM_GPIO_MOTOR_DIR_LEFT : 0;
  }
  if(gpio & PLATFORM_GPIO_MOTOR_ENABLE)
  {
    value |= mcual_gpio_get(MCUAL_GPIOE, MCUAL_GPIO_PIN2) ? PLATFORM_GPIO_MOTOR_ENABLE : 0;
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

#ifdef CONFIG_OS_USE_FREERTOS
void platform_spi_slave_select(uint8_t select)
{
  if(select != PLATFORM_SPI_CS_UNSELECT)
  {
    xSemaphoreTake(mutex_spi_slave, portMAX_DELAY);
  }

  mcual_gpio_set(MCUAL_GPIOD, select & 0x0F); 
  mcual_gpio_clear(MCUAL_GPIOD, (~select) & 0x0F); 

  if(select == PLATFORM_SPI_CS_UNSELECT)
  {
    xSemaphoreGive(mutex_spi_slave);
  }
}
#endif

uint8_t platform_spi_slave_transfert(uint8_t data)
{
  return mcual_spi_master_transfert(MCUAL_SPI3, data);
}

#ifdef CONFIG_OS_USE_FREERTOS
void platform_spi_position_select(uint8_t select)
{
  //reset chip select
  mcual_gpio_set(MCUAL_GPIOD, (1 << 8) | (1 << 9) | (1 << 10));

  //clear the good one
  switch(select)
  {
    case PLATFORM_SPI_ENCR_SELECT:
      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
      mcual_gpio_clear(MCUAL_GPIOD, (1 << 9));
      break;

    case PLATFORM_SPI_ENCL_SELECT:
      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
      mcual_gpio_clear(MCUAL_GPIOD, (1 << 8));
      break;

    case PLATFORM_SPI_ENCG_SELECT:
      xSemaphoreTake(mutex_spi_position, portMAX_DELAY);
      mcual_gpio_clear(MCUAL_GPIOD, (1 << 10));
      break;

    default:
      xSemaphoreGive(mutex_spi_position);
      break;
  }
}
#endif

uint8_t platform_spi_position_transfert(uint8_t data)
{
  return mcual_spi_master_transfert(MCUAL_SPI2, data);
}

#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_frequency(uint32_t freq_Hz)
{
  mcual_timer_init(MCUAL_TIMER9, freq_Hz);
}
#endif

#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_left_duty_cycle(uint32_t duty_cycle)
{
  mcual_timer_set_duty_cycle(MCUAL_TIMER9, MCUAL_TIMER_CHANNEL1, duty_cycle);
}
#endif

#ifdef CONFIG_MCUAL_TIMER
void platform_motor_set_right_duty_cycle(uint32_t duty_cycle)
{
  mcual_timer_set_duty_cycle(MCUAL_TIMER9, MCUAL_TIMER_CHANNEL2, duty_cycle);
}
#endif

void platform_servo_set_value(uint32_t servo_id, uint32_t value)
{
  pcm9685_set_channel(servo_id, 0, value);
}

uint8_t platform_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
{
  xSemaphoreTake(mutex_i2c, portMAX_DELAY);
  uint8_t ret = mcual_i2c_transmit(id, addr, txbuf, tx_size, rxbuf, rx_size);
  xSemaphoreGive(mutex_i2c);

  return ret;
}
