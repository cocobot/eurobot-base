#include <stdio.h>
#include <platform.h>
#include <mcual.h>

#define MECA_CRIMP_ENABLE      PLATFORM_GPIO1
#define MECA_SUCKER_LEFT_PUMP   PLATFORM_GPIO3
#define MECA_SUCKER_LEFT_SUCKER PLATFORM_GPIO2
#define MECA_SUCKER_RIGHT_PUMP   PLATFORM_GPIO5
#define MECA_SUCKER_RIGHT_SUCKER PLATFORM_GPIO4


static void event(void)
{
  platform_led_toggle(PLATFORM_LED0);
}

int main(void) 
{
  platform_init();

  /*
  platform_gpio_set_direction(MECA_CRIMP_ENABLE, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_LEFT_PUMP, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_LEFT_SUCKER, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_RIGHT_PUMP, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_RIGHT_SUCKER, MCUAL_GPIO_OUTPUT);

  platform_gpio_clear(MECA_CRIMP_ENABLE);
  platform_gpio_clear(MECA_SUCKER_LEFT_PUMP);
  platform_gpio_clear(MECA_SUCKER_LEFT_SUCKER);
  platform_gpio_clear(MECA_SUCKER_RIGHT_PUMP);
  platform_gpio_clear(MECA_SUCKER_RIGHT_SUCKER);
  */



  mcual_loader_init(MCUAL_USART1, event);
  mcual_loader_run();

  return 0;
}
