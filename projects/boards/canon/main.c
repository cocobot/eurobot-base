#include <platform.h>
#include <cocobot.h>
#include "motor_control.h"

 
int main(void) {
  //initialisations of mcual and libcocobot
  platform_init();
  cocobot_com_init();
  cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);
  motor_control_init();

  //main loop
  for(;;)  {
    //process communication requests
    uint64_t timestamp_us = cocobot_com_process_event();

    //run motor control algorithm
    motor_control_process_event(timestamp_us);
  }

  return 0;
}
