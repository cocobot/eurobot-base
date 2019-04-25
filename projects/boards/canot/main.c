#include <platform.h>
#include <cocobot.h>
 
int main(void) 
{
  platform_init();

  cocobot_com_init();

  cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE);
  while(1)
  {
    cocobot_com_process_event();
  }

  return 0;
}
