#include <cocobot.h>
#include <stdio.h>
#include "servo.h"
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pump.h"

static cocobot_arm_t arm[4] = {0};

void cocobot_arm_action_init(void)
{
  for(int i = 0; i <= 2; i++)
  {
      cocobot_arm_action_repos_vide(i);
  }
}

void cocobot_arm_action_repos_vide(int arm_id)
{
  switch(arm_id)
  {
    case 0:
      servo_set_angle(0, 80);
      servo_set_angle(1, -140);
      servo_set_angle(2, -20);
      break;

    case 1:
      servo_set_angle(3, 85);
      servo_set_angle(4, -140);
      servo_set_angle(5, -20);
      break;
  }
}

void cocobot_arm_action_repos_normal(int arm_id)
{
  switch(arm_id)
  {
    case 0:
      servo_set_angle(0, 70);
      servo_set_angle(1, -120);
      servo_set_pwm(2, 0);
      break;

    case 1:
      servo_set_angle(3, 70);
      servo_set_angle(4, -120);
      servo_set_pwm(5, 0);
      break;
  }
}

void cocobot_arm_action_prise_distributeur(int arm_id, float angle)
{
  switch(arm_id)
  {
    case 0:
      servo_set_angle(0, 40);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(2, 65);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -110);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -110);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      break;

    case 1:
      servo_set_angle(3, 35);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(5, 60);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -110);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -110);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
   
      break;
  }
}

void cocobot_arm_action_depose_balance(int arm_id, float angle)
{
  switch(arm_id)
  {
    case 0:
      servo_set_angle(0, 80);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -130);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(2, 70);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(0, 65);
      vTaskDelay(250/portTICK_PERIOD_MS);
      pump_set_state(arm_id, 0);
      vTaskDelay(1500/portTICK_PERIOD_MS);
      break;

    case 1:
servo_set_angle(0, 80);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -130);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(2, 70);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(1, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(0, 65);
      vTaskDelay(250/portTICK_PERIOD_MS);
      pump_set_state(arm_id, 0);
      vTaskDelay(1500/portTICK_PERIOD_MS);
 
      break;
  }
}

/*
void cocobot_arm_action_prendre_palais_sol(int arm_id, float x, float y)
{
    if(arm_id == 0)
    {
        servo_set_angle(2, 5);
        servo_set_angle(1, -110);
        servo_set_angle(0, 10);
       return; 
    }
    if(arm_id == 2)
    {
        servo_set_angle(8, 5);
        servo_set_angle(7, -110);
        servo_set_angle(6, 10);
       return; 
    }
if(arm_id == 3)
    {
        servo_set_angle(9, 30);
        servo_set_angle(11, -65);
        servo_set_angle(10, -20);

        vTaskDelay(500/portTICK_PERIOD_MS);
        servo_set_angle(9, -15);
       return; 
    }
if(arm_id == 1)
    {
        servo_set_angle(3, 15);
        servo_set_angle(4, -110);
        servo_set_angle(5, 10);
       return; 
    }
vTaskDelay(200/portTICK_PERIOD_MS);
        servo_set_pwm(arm_id*3 + 2, 0);
}

void cocobot_arm_action_repos_vide(int arm_id)
{
    if(arm_id == 3)
    {
servo_set_angle(arm_id*3, 80);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 1, -140);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 2, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);
    return;
    }
    servo_set_angle(arm_id*3, 80);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 1, -150);
    vTaskDelay(100/portTICK_PERIOD_MS);
    servo_set_angle(arm_id*3 + 2, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);
}



void cocobot_arm_action_repos_normal(int arm_id)
{
  cocobot_arm_action_repos_vide(arm_id);
}

void cocobot_arm_action_pos_stockage_palais(int arm_id)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_prise_distributeur(int arm_id, float angle)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_orientation(int arm_id, int side)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_prise_goldenium(int arm_id, float angle)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_repos_goldenium(int arm_id)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_prise_bluenium(int arm_id, float angle)
{
    servo_set_angle(7, -70);
    servo_set_angle(8, 20);

    vTaskDelay(500/portTICK_PERIOD_MS);

    servo_set_angle(6, 50);
}



void cocobot_arm_action_depose_accelerateur_particules(int arm_id, float angle, float distance)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_depose_case(int arm_id, float angle)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
      servo_set_angle(arm_id*3, 50);
      servo_set_angle(arm_id*3 + 1, -80);
      servo_set_angle(arm_id*3 + 2, -30);
    // TODO: à régler !
    //cocobot_arm_move_arti(arm[arm_id], a1_deg, a2_deg, a3_deg, a4_deg);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_move_arm(int arm_id, float x, float y, float z, float a)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    cocobot_arm_move_cartesian(&arm[arm_id], x, y, z, a);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_print_pos(int arm_id)
{
  if (arm_id >= 0 && arm_id <= 3)
  {
    cocobot_arm_print_pos(&arm[arm_id]);
  }
  else
  {
    printf("Wrong arm_id: %d should be between 0 and 3.\n", arm_id);
  }
}

void cocobot_arm_action_test_and_print(void)
{
  printf("\n-------------------- Move arm 0 arti (0, 0, 0, 0):\n");
  cocobot_arm_move_arti(&arm[0], 0, 0, 0, 0);
  cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 1 arti (0, 0, 0, 0):\n");
  cocobot_arm_move_arti(&arm[1], 0, 0, 0, 0);
  cocobot_arm_action_print_pos(1);
  printf("\n-------------------- Move arm 0 cart (0.3334, -0.012, 0.097, 0)\n");
  cocobot_arm_action_move_arm(0, 0.3334, -0.012, 0.097, 0);
  cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 1 cart (0.3334, -0.012, 0.097, 0)\n");
  cocobot_arm_action_move_arm(1, 0.3334, -0.012, 0.097, 0);
  cocobot_arm_action_print_pos(1);
  printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
  cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 1 cart  (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_action_move_arm(1, 0.23, -0.023, 0.13, -42);
  cocobot_arm_action_print_pos(1);
}

void cocobot_arm_action_test_multiple_move_arm(void)
{
  printf("========== TEST PRINT ==========\n\n");
  printf("\n-------------------- Move arm 0 arti (0, 0, 0, 0):\n");
  cocobot_arm_move_arti(&arm[0], 0, 0, 0, 0);
  //cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
  //cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
  //cocobot_arm_action_print_pos(0);
  printf("\n-------------------- Move arm 0 cart  (0.23, -0.023, 0.13, -42)\n");
  cocobot_arm_action_move_arm(0, 0.23, -0.023, 0.13, -42);
  //cocobot_arm_action_print_pos(0);
  printf("\n\n==========END TEST PRINT ==========\n\n");
}
*/
