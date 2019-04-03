#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();
  cocobot_position_init(4);

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1225 - 60);
      cocobot_position_set_y(800);
      cocobot_position_set_angle(0);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1225 + 60);
      cocobot_position_set_y(800);
      cocobot_position_set_angle(180);
      break;
  }

  vTaskStartScheduler();

  return 0;
}
