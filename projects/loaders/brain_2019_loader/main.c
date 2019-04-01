#include <platform.h>
#include <cocobot.h>
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_loader_init();

  return 0;
}
