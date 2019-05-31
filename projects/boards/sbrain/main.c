#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>

#define DEMI_LARGEUR_ROBOT      150
#define DEMI_LONGUEUR_ARR_ROBOT 107
#define MARGE_PRISE_BRAS        100

extern cocobot_pathfinder_table_init_s initTable [];

void run_homologation(void * arg)
{
  (void)arg;

  cocobot_game_state_wait_for_configuration();
  
  //set initial position

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1350);
      cocobot_position_set_y(550);
      cocobot_position_set_angle(90);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1350);
      cocobot_position_set_y(550);
      cocobot_position_set_angle(90);
      break;
  }

  vTaskDelay(500 / portTICK_PERIOD_MS); 
    
  cocobot_game_state_wait_for_starter_removed();
  cocobot_game_state_add_points_to_score(1);

  //sort de la zone vers l'experience
  cocobot_trajectory_goto_d(150, 4000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //tourne face a l'autre equipe
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
      cocobot_trajectory_goto_a(0, 4000);
  }
  else
  {
      cocobot_trajectory_goto_a(180, 4000);
  }

  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //Avance de 20cm
  cocobot_trajectory_goto_d(300, 4000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //tourne face à la balance
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
      cocobot_trajectory_goto_a(-90, 4000);
  }
  else
  {
      cocobot_trajectory_goto_a(-90, 4000);
  }
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //Avance de 20cm
  cocobot_trajectory_goto_d(200, 4000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //tourne face à la balance
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
  {
      cocobot_trajectory_goto_a(180, 4000);
  }
  else
  {
      cocobot_trajectory_goto_a(0, 4000);
  }
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  //Avance de 30cm
  cocobot_trajectory_goto_d(300, 4000);
  cocobot_trajectory_wait();
  cocobot_game_state_add_points_to_score(1);

  

  //cocobot_trajectory_goto_xy(500, 0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //cocobot_trajectory_goto_xy(500, 500, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //cocobot_trajectory_goto_xy(0, 500, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //cocobot_trajectory_goto_xy(0, 0, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
  //cocobot_trajectory_wait();

  while(1)
  {
    vTaskDelay(100/portTICK_PERIOD_MS);
  }

  while(1)
  {
    //do nothing !
    uprintf("x=%d y=%d a=%d d=%d -- l=%ld r=%ld\r\n", 
            (int)cocobot_position_get_x(),
            (int)cocobot_position_get_y(),
            (int)cocobot_position_get_angle(),
            (int)cocobot_position_get_distance(),
            (int)cocobot_position_get_left_encoder(),
            (int)cocobot_position_get_right_encoder());

    cocobot_com_printf(COM_DEBUG, "x=%d y=%d a=%d d=%d -- l=%ld r=%ld\r\n", 
            (int)cocobot_position_get_x(),
            (int)cocobot_position_get_y(),
            (int)cocobot_position_get_angle(),
            (int)cocobot_position_get_distance(),
            (int)cocobot_position_get_left_encoder(),
            (int)cocobot_position_get_right_encoder());

    vTaskDelay(100/portTICK_PERIOD_MS);
  }

  while(1)
    vTaskDelay(100/portTICK_PERIOD_MS);
}

void run_strategy(void * arg)
{
  cocobot_game_state_wait_for_configuration();

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1050 - DEMI_LARGEUR_ROBOT);
      cocobot_position_set_y(-543 + DEMI_LONGUEUR_ARR_ROBOT);
      cocobot_position_set_angle(90);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1050 - DEMI_LARGEUR_ROBOT);
      cocobot_position_set_y(-543 + DEMI_LONGUEUR_ARR_ROBOT);
      cocobot_position_set_angle(90);
      break;
  }

  //init tempo
  vTaskDelay(500 / portTICK_PERIOD_MS); 

  //attente tirette présente
  while(!platform_gpio_get(PLATFORM_GPIO_STARTER))
  {
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }

  //déplacement dans la zone de départ
  cocobot_asserv_set_angle_activation(0); //on se sort de la barriere
  cocobot_trajectory_goto_d(150, 5000);
  cocobot_trajectory_wait();
  cocobot_asserv_set_angle_activation(1); //on se sort de la barriere

  //on se place dans la zone de départ
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy(-1500 + 450 - 28 - DEMI_LARGEUR_ROBOT, 100 + DEMI_LONGUEUR_ARR_ROBOT + 28, 15000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy(1500 - 450 + 28 + DEMI_LARGEUR_ROBOT, 100 + DEMI_LONGUEUR_ARR_ROBOT + 28, 15000);
      break;
  }
  cocobot_trajectory_goto_a(90, 4000);
  cocobot_trajectory_wait();

  //ici on est dans la zone de départ.
  //on laisse libre le mouvement du robot
  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
  

  //La tirette !
  cocobot_game_state_wait_for_starter_removed();

  //C'est parti !


  //on se positionne devant le Vert 1
  //
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy(-450 + 50 + 100, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(180, 4000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy(450 -(50 + 100), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(0, 4000);
      break;
  }
  cocobot_trajectory_wait();

  //on va vers le bleu
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy(-450 + 50 + 300, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(180, 4000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy(450 -(50 + 300), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(0, 4000);
      break;
  }
  cocobot_trajectory_wait();

  //prise !
  //
  vTaskDelay(3000 / portTICK_PERIOD_MS);




  //fin !
  while(1)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }

}
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_opponent_detection_init(3);
  cocobot_game_state_init(NULL);
  cocobot_pathfinder_init(initTable);
  cocobot_action_scheduler_use_pathfinder(1);

  cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);

  //set initial position
  cocobot_position_set_x(0);
  cocobot_position_set_y(0);
  cocobot_position_set_angle(0);

  //xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );
  xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );

  vTaskStartScheduler();

  return 0;
}
