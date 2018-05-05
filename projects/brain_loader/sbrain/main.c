#include <stdio.h>
#include <platform.h>
#include <mcual.h>

static void event(void)
{
  platform_led_toggle(PLATFORM_LED0);
}

int main(void) 
{
  platform_init();

  platform_gpio_set_direction(PLATFORM_GPIO_MOTOR_ENABLE, MCUAL_GPIO_OUTPUT);
  platform_gpio_set(PLATFORM_GPIO_MOTOR_ENABLE);

  mcual_loader_init(MCUAL_USART1, event);
  mcual_loader_run();

  return 0;
}
