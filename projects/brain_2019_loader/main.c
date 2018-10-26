#include <platform.h>
#include <cocobot.h>

//static void event(void)
//{
//  platform_led_toggle(PLATFORM_LED0);
//}

int main(void) 
{
  platform_init();

  cocobot_can_init();

  return 0;
}
