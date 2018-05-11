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
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN4, MCUAL_GPIO_INPUT); //GPIO0
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN5, MCUAL_GPIO_INPUT); //GPIO1
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN6, MCUAL_GPIO_INPUT); //GPIO2
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN7, MCUAL_GPIO_INPUT); //GPIO3
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN0, MCUAL_GPIO_OUTPUT);//MS1
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);//MS2
  mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN2, MCUAL_GPIO_OUTPUT);//MS3
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN0, MCUAL_GPIO_OUTPUT);//M0 DIR
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);//M0 STEP
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN2, MCUAL_GPIO_OUTPUT);//M1 DIR
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN3, MCUAL_GPIO_OUTPUT);//M1 STEP
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN4, MCUAL_GPIO_OUTPUT);//M2 DIR
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN5, MCUAL_GPIO_OUTPUT);//M2 STEP
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN6, MCUAL_GPIO_OUTPUT);//M3 DIR
  mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN7, MCUAL_GPIO_OUTPUT);//M3 STEP
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, MCUAL_GPIO_OUTPUT);//ENABLE
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN1, MCUAL_GPIO_OUTPUT);//RESET
  mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN2, MCUAL_GPIO_OUTPUT);//SLEEP

  //disable stepper by default
  platform_gpio_set(PLATFORM_STEPPER_ENABLE);
  platform_gpio_clear(PLATFORM_STEPPER_RESET);
  platform_gpio_clear(PLATFORM_STEPPER_SLEEP);
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
  if(gpio & PLATFORM_STEPPER_MS1)
  {
    mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN0, direction);
  }
  if(gpio & PLATFORM_STEPPER_MS2)
  {
    mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN1, direction);
  }
  if(gpio & PLATFORM_STEPPER_MS3)
  {
    mcual_gpio_init(MCUAL_GPIOB, MCUAL_GPIO_PIN2, direction);
  }
  if(gpio & PLATFORM_STEPPER_ENABLE)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN0, direction);
  }
  if(gpio & PLATFORM_STEPPER_RESET)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN1, direction);
  }
  if(gpio & PLATFORM_STEPPER_SLEEP)
  {
    mcual_gpio_init(MCUAL_GPIOA, MCUAL_GPIO_PIN2, direction);
  }
  if(gpio & PLATFORM_STEPPER_M0_DIR)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN0, direction);
  }
  if(gpio & PLATFORM_STEPPER_M0_STEP)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN1, direction);
  }
  if(gpio & PLATFORM_STEPPER_M1_DIR)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN2, direction);
  }
  if(gpio & PLATFORM_STEPPER_M1_STEP)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN3, direction);
  }
  if(gpio & PLATFORM_STEPPER_M2_DIR)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN4, direction);
  }
  if(gpio & PLATFORM_STEPPER_M2_STEP)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN5, direction);
  }
  if(gpio & PLATFORM_STEPPER_M3_DIR)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN6, direction);
  }
  if(gpio & PLATFORM_STEPPER_M3_STEP)
  {
    mcual_gpio_init(MCUAL_GPIOC, MCUAL_GPIO_PIN7, direction);
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
  if(gpio & PLATFORM_STEPPER_MS1)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_MS2)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_MS3)
  {
    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_ENABLE)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_RESET)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_SLEEP)
  {
    mcual_gpio_set(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M0_DIR)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_M0_STEP)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_M1_DIR)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M1_STEP)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_STEPPER_M2_DIR)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_STEPPER_M2_STEP)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_STEPPER_M3_DIR)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_STEPPER_M3_STEP)
  {
    mcual_gpio_set(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
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
  if(gpio & PLATFORM_STEPPER_MS1)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_MS2)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_MS3)
  {
    mcual_gpio_clear(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_ENABLE)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_RESET)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_SLEEP)
  {
    mcual_gpio_clear(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M0_DIR)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_M0_STEP)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_M1_DIR)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M1_STEP)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_STEPPER_M2_DIR)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_STEPPER_M2_STEP)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_STEPPER_M3_DIR)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_STEPPER_M3_STEP)
  {
    mcual_gpio_clear(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
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
if(gpio & PLATFORM_STEPPER_MS1)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_MS2)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_MS3)
  {
    mcual_gpio_toggle(MCUAL_GPIOB, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_ENABLE)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_RESET)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_SLEEP)
  {
    mcual_gpio_toggle(MCUAL_GPIOA, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M0_DIR)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN0);
  }
  if(gpio & PLATFORM_STEPPER_M0_STEP)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN1);
  }
  if(gpio & PLATFORM_STEPPER_M1_DIR)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN2);
  }
  if(gpio & PLATFORM_STEPPER_M1_STEP)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN3);
  }
  if(gpio & PLATFORM_STEPPER_M2_DIR)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  }
  if(gpio & PLATFORM_STEPPER_M2_STEP)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN5);
  }
  if(gpio & PLATFORM_STEPPER_M3_DIR)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN6);
  }
  if(gpio & PLATFORM_STEPPER_M3_STEP)
  {
    mcual_gpio_toggle(MCUAL_GPIOC, MCUAL_GPIO_PIN7);
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
  if(gpio & PLATFORM_STEPPER_MS1)
  {
    value |= mcual_gpio_get(MCUAL_GPIOB, MCUAL_GPIO_PIN0) ? PLATFORM_STEPPER_MS1 : 0;
  }
  if(gpio & PLATFORM_STEPPER_MS2)
  {
    value |= mcual_gpio_get(MCUAL_GPIOB, MCUAL_GPIO_PIN1) ? PLATFORM_STEPPER_MS2 : 0;
  }
  if(gpio & PLATFORM_STEPPER_MS3)
  {
    value |= mcual_gpio_get(MCUAL_GPIOB, MCUAL_GPIO_PIN2) ? PLATFORM_STEPPER_MS3 : 0;
  }
  if(gpio & PLATFORM_STEPPER_ENABLE)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN0) ? PLATFORM_STEPPER_ENABLE : 0;
  }
  if(gpio & PLATFORM_STEPPER_RESET)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN1) ? PLATFORM_STEPPER_RESET : 0;
  }
  if(gpio & PLATFORM_STEPPER_SLEEP)
  {
    value |= mcual_gpio_get(MCUAL_GPIOA, MCUAL_GPIO_PIN2) ? PLATFORM_STEPPER_SLEEP : 0;
  }
  if(gpio & PLATFORM_STEPPER_M0_DIR)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN0) ? PLATFORM_STEPPER_M0_DIR : 0;
  }
  if(gpio & PLATFORM_STEPPER_M0_STEP)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN1) ? PLATFORM_STEPPER_M0_STEP : 0;
  }
  if(gpio & PLATFORM_STEPPER_M1_DIR)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN2) ? PLATFORM_STEPPER_M1_DIR : 0;
  }
  if(gpio & PLATFORM_STEPPER_M1_STEP)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN3) ? PLATFORM_STEPPER_M1_STEP : 0;
  }
  if(gpio & PLATFORM_STEPPER_M2_DIR)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN4) ? PLATFORM_STEPPER_M2_DIR : 0;
  }
  if(gpio & PLATFORM_STEPPER_M2_STEP)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN5) ? PLATFORM_STEPPER_M2_STEP : 0;
  }
  if(gpio & PLATFORM_STEPPER_M3_DIR)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN6) ? PLATFORM_STEPPER_M3_DIR : 0;
  }
  if(gpio & PLATFORM_STEPPER_M3_STEP)
  {
    value |= mcual_gpio_get(MCUAL_GPIOC, MCUAL_GPIO_PIN7) ? PLATFORM_STEPPER_M3_STEP : 0;
  }

  return value;
}
