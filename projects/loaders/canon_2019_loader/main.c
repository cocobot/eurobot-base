#include <platform.h>
#include <cocobot.h>
 
int main(void) 
{
  platform_init();
  platform_led_set(PLATFORM_LED0);

  cocobot_com_init();
  cocobot_loader_init();

  return 0;
}
