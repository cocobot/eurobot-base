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

  cocobon_can_init();

  mcual_loader_init(MCUAL_USART1, event);
  mcual_loader_run();

  return 0;
}
