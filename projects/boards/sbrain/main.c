#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "meca.h"

#define DEMI_LARGEUR_ROBOT      150
#define DEMI_LONGUEUR_ARR_ROBOT 107
#define MARGE_PRISE_BRAS        100
#define MARGE_DEPOSE_BRAS       100

extern cocobot_pathfinder_table_init_s initTable [];

void stop(void)
{
  cocobot_trajectory_wait();
  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);
  while(1)
  {
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void stop_meca(void)
{
  meca_action(0, MECA_STOP);
}

void meca_take_special(uint8_t arm)
{
  meca_action(arm, MECA_TAKE_DISTRIB_SPECIAL);

  ///vTaskDelay(1500 / portTICK_PERIOD_MS); 

  ///float a = cocobot_posuition_get_a();
  ///cocobot_trajectory_goto_a(a + 5, 2000);
  ///cocobot_trajectory_wait();
  ///cocobot_trajectory_goto_a(a - 5, 2000);
  ///cocobot_trajectory_wait();

  ///meca_action(arm, MECA_TAKE_REST_EMPTY);
}

/*
static void cocobot_callage_backward()
{
    cocobot_trajectory_result_t result;
    //no angle
    cocobot_asserv_set_angle_activation(0);
    cocobot_asserv_big_accel_d();
    //move backard until it is blocked
    do
    {
        result = cocobot_trajectory_wait();
    }
    while(result == COCOBOT_TRAJECTORY_SUCCESS); 
    cocobot_asserv_normal();

    //angle is back
}
*/

void depose_balance(void)
{
  //on se place a la fin du distributeur

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1050 + 50 + 600, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_xy_backward(-272, -600 + DEMI_LARGEUR_ROBOT + MARGE_DEPOSE_BRAS, 4000);
      cocobot_trajectory_goto_a(180, 5000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1050 -(50 + 600), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_xy_backward(272, -600 + DEMI_LARGEUR_ROBOT + MARGE_DEPOSE_BRAS, 4000);
      cocobot_trajectory_goto_a(0, 5000);
      break;
  }
  cocobot_trajectory_wait();

  cocobot_asserv_slow();
  cocobot_trajectory_goto_d(-50, 3000);
  cocobot_trajectory_goto_d(-100, 1000);
  cocobot_trajectory_wait();
  cocobot_asserv_normal();

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      meca_action(0, MECA_DROP_BALANCE);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      meca_action(1, MECA_DROP_BALANCE);
      break;
  }
}

void depose_balance_double(void)
{
  //on se place a la fin du distributeur

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1050 + 50 + 600, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_xy_backward(-272, -600 + DEMI_LARGEUR_ROBOT + MARGE_DEPOSE_BRAS, 4000);
      cocobot_trajectory_goto_a(180, 5000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1050 -(50 + 600), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_xy_backward(272, -600 + DEMI_LARGEUR_ROBOT + MARGE_DEPOSE_BRAS, 4000);
      cocobot_trajectory_goto_a(0, 5000);
      break;
  }
  cocobot_trajectory_wait();

  cocobot_asserv_slow();
  cocobot_trajectory_goto_d(-50, 3000);
  cocobot_trajectory_goto_d(-100, 1000);
  cocobot_trajectory_wait();
  cocobot_asserv_normal();


  //Depose
  meca_action(0, MECA_DROP_BALANCE);

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_game_state_add_points_to_score(12); 
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_game_state_add_points_to_score(8); 
      break;
  }

  cocobot_trajectory_goto_a(180, 5000);
  cocobot_trajectory_wait();

  //Depose
  meca_action(1, MECA_DROP_BALANCE);
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_game_state_add_points_to_score(8); 
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_game_state_add_points_to_score(12); 
      break;
  }
  
  //on s'en va. On verra plus tard pour les rouges
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50 + 100, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1425 -(50 + 100), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }
}

void sortie_balance(void)
{
  cocobot_trajectory_goto_d(150, 4000);
  cocobot_trajectory_wait();

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1050 + 50 + 600, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1050 -(50 + 600), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }

  cocobot_trajectory_wait();
}

void prise_et_depose_6(uint8_t numero, uint8_t mode)
{
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      {
      float target_x = -1050 + 50 + numero * 100;
      if(target_x > cocobot_position_get_x())
      {
        cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);
      }
      else
      {
        cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);
      }
      cocobot_trajectory_goto_xy(target_x, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(180, 4000);
      }
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      {
        float target_x = 1050 - (50 + numero * 100);
        if(target_x > cocobot_position_get_x())
        {
          cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_BACKWARD);
        }
        else
        {
          cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);
        }
        cocobot_trajectory_goto_xy(target_x, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
        cocobot_trajectory_goto_a(0, 4000);
      }
      break;
  }
  cocobot_trajectory_wait();
  cocobot_trajetory_set_xy_default(COCOBOT_TRAJECTORY_FORWARD);

  if(mode == 0)
  {
    //stop();
    //prise
    switch(cocobot_game_state_get_color())
    {
      case COCOBOT_GAME_STATE_COLOR_NEG:
        meca_action(0, MECA_TAKE_DISTRIB);
        break;

      case COCOBOT_GAME_STATE_COLOR_POS:
        meca_action(1, MECA_TAKE_DISTRIB);
        break;
    }

    //on va le poser dans la balance
    depose_balance();
    switch(numero)
    {
      case 0:
      case 2:
      case 4:
        cocobot_game_state_add_points_to_score(4); 
        break;

      case 1:
      case 5:
        cocobot_game_state_add_points_to_score(8); 
        break;

      case 3:
        cocobot_game_state_add_points_to_score(12); 
        break;
    }

    sortie_balance();
  }
}


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
  (void)arg;
  cocobot_game_state_wait_for_configuration();
  cocobot_trajectory_set_opponent_detection(0);

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1050 + DEMI_LARGEUR_ROBOT);
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
  
  vTaskDelay(1000 / portTICK_PERIOD_MS); 
  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);


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
  
  //On dégage le chemin sans pourrir le travail du gros
  cocobot_trajectory_goto_xy_backward(cocobot_position_get_x(), -300, 5000);

  //on se positionne devant le Vert 1
  //
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1050 + 50 + 100, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(180, 4000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1050 -(50 + 100), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(0, 4000);
      break;
  }
  cocobot_trajectory_wait();

  //on va vers le bleu
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1050 + 50 + 300, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(180, 4000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1050 -(50 + 300), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      cocobot_trajectory_goto_a(0, 4000);
      break;
  }
  cocobot_trajectory_wait();

  //on prend le bleu
  prise_et_depose_6(3, 0);
  prise_et_depose_6(5, 0);
  prise_et_depose_6(1, 0);

  //on va vers le pack de 3
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50 + 100, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1425 -(50 + 100), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50 + 100, -1000 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1425 -(50 + 100), -1000 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }
  cocobot_trajectory_goto_a(90, 5000);
  cocobot_trajectory_wait();

  //petit callage opportun
  cocobot_trajectory_goto_d(-100, 4000);
  cocobot_trajectory_goto_d(-100, 2000);
  cocobot_trajectory_wait();

  //stabilisation de la position
  vTaskDelay(500 / portTICK_PERIOD_MS); 

  float y_3_offset = cocobot_position_get_y() - DEMI_LONGUEUR_ARR_ROBOT;

  //on se degage
  cocobot_trajectory_goto_d(200, 4000);

  //on se place pour le vert
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50 + 100, y_3_offset + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1425 -(50 + 100), y_3_offset + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }
  cocobot_trajectory_goto_a(0, 5000);
  cocobot_trajectory_wait();

  //prise vert
  meca_action(1, MECA_TAKE_DISTRIB);

  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50, y_3_offset + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy(1425 -(50), y_3_offset + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }

  cocobot_trajectory_goto_a(180, 5000);
  cocobot_trajectory_wait();

  //prise bleu
  meca_action(0, MECA_TAKE_DISTRIB);

  //on se dégage du board
  cocobot_trajectory_goto_d(100, 5000);
  cocobot_trajectory_wait();
  
  //on retourne vers la balance
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_xy_backward(-1425 + 50 + 100, -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_xy_backward(1425 -(50 + 100), -543 + DEMI_LARGEUR_ROBOT + MARGE_PRISE_BRAS, 10000);
      break;
  }

  depose_balance_double();


  //On prend un rouge
  prise_et_depose_6(4, 1);
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      meca_action(0, MECA_TAKE_DISTRIB);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      meca_action(1, MECA_TAKE_DISTRIB);
      break;
  }

  //Puis un second
  prise_et_depose_6(4, 1);
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_trajectory_goto_a(0, 5000);
      cocobot_trajectory_wait();
      meca_action(1, MECA_TAKE_DISTRIB);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_trajectory_goto_a(180, 5000);
      cocobot_trajectory_wait();
      meca_action(0, MECA_TAKE_DISTRIB);
      break;
  }



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
  cocobot_game_state_init(stop_meca);
  cocobot_pathfinder_init(initTable);
  cocobot_action_scheduler_use_pathfinder(1);
  meca_init();

  cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);

  //set initial position
  cocobot_position_set_x(0);
  cocobot_position_set_y(0);
  cocobot_position_set_angle(0);

  (void)run_homologation;

  //xTaskCreate(run_homologation, "strat", 600, NULL, 2, NULL );
  xTaskCreate(run_strategy, "strat", 600, NULL, 2, NULL );

  vTaskStartScheduler();

  return 0;
}
