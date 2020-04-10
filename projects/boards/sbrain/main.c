#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "meca.h"

//2020 table data
extern cocobot_pathfinder_table_init_s initTable[];

/**
 * @brief Main task for brain board
 * @param arg   Not used
 */
void run_strategy(void * arg)
{
  (void)arg;

  //wait remote data
  cocobot_game_state_wait_for_configuration();

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
    cocobot_position_set_x(-1190);
    cocobot_position_set_y(1200);
    cocobot_position_set_angle(0);
    break;

    case COCOBOT_GAME_STATE_COLOR_POS:
    cocobot_position_set_x(1190);
    cocobot_position_set_y(1200);
    cocobot_position_set_angle(180);
    break;
  }

  //wait start !
  cocobot_game_state_wait_for_starter_removed();

  //set square
  while(1)
  {
    cocobot_trajectory_goto_xy(-500, 1500, 30000);
    cocobot_trajectory_goto_xy(500, 1500, 30000);
    cocobot_trajectory_goto_xy(500, 500, 30000);
    cocobot_trajectory_goto_xy(-500, 500, 30000);
    cocobot_trajectory_wait();

    vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }
}


/**
 * @brief Raise the flag before the end of the match
 */
void set_flag_up(void)
{
  cocobot_com_send(COCOBOT_COM_MECA_ACTION_PID, "B", 25); //back up
  cocobot_game_state_add_points_to_score(10);
}



/**
 * @brief Handle packet from Cocoui or other boards if libcocobot didn't want it
 * @param pid   PID of the packet
 * @param data  buffer for packet data
 * @param len   len of packet data
 */
void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  (void)pid;
  (void)data;
  (void)len;
}


int main(int argc, char *argv[]) 
{
#ifdef AUSBEE_SIM
  mcual_arch_main(argc, argv);
#else
  (void)argc;
  (void)argv;
#endif
  platform_init();

  cocobot_com_init(MCUAL_USART1, 1, 1, com_handler);
  cocobot_position_init(4);
  //cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  //cocobot_opponent_detection_init(3);
  cocobot_game_state_init(set_flag_up);
  //cocobot_pathfinder_init(initTable);
  //cocobot_action_scheduler_use_pathfinder(1);
  //meca_init();


  xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );
  vTaskStartScheduler();

  return 0;
}
