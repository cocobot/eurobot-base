#include <cocobot.h>
#include <stdio.h>
#include "servo.h"
#include "cocobot_arm.h"
#include "cocobot_arm_action.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pump.h"

// Servo 0 = Mat drapeau low = 470, high = 115
// Servo 1 = manche a air droite (opposé au mat drapeau) low = 400, up = 200 
// servo 2 = pince av droite ouvert = , fermé =  

void cocobot_arm_action_init(void)
{
  //Flag
  cocobot_arm_action_flag_down();

  //windsock
  cocobot_arm_action_windsock_up(WINDSOCK_RIGHT);
  cocobot_arm_action_windsock_up(WINDSOCK_LEFT);

  //pliers
  //Using a loop is ok because value of the enum starts at 0
  for(int i = 0; i < 6; i++)
    cocobot_arm_action_pliers_close(i);

  cocobot_arm_action_pliers_up(PLIERS_CARRIER_FRONT);
  cocobot_arm_action_pliers_up(PLIERS_CARRIER_BACK);
}
  
void cocobot_arm_action_flag_down()
{
  //No position, because risk of breaking the flag carrier
  servo_set_pwm(0, 0);
}

void cocobot_arm_action_flag_up()
{
  servo_set_pwm(0, 115);
}

void cocobot_arm_action_windsock_down(windsock_e windsock)
{
  switch(windsock)
  {
    case WINDSOCK_RIGHT: 
      servo_set_pwm(1, 400);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case WINDSOCK_LEFT: 
      servo_set_pwm(8, 390);
      vTaskDelay(250/portMAX_DELAY);
      break;
  }
}

void cocobot_arm_action_windsock_up(windsock_e windsock)
{
  switch(windsock)
  {
    case WINDSOCK_RIGHT:
      servo_set_pwm(1, 200);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case WINDSOCK_LEFT:
      servo_set_pwm(8, 220);
      vTaskDelay(250/portMAX_DELAY);
      break;
  }
}

void cocobot_arm_action_pliers_close(pliers_e pliers)
{
  switch (pliers)
  {
    case PLIERS_FRONT_RIGHT :
      servo_set_pwm(2, 230);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_FRONT_LEFT :
      servo_set_pwm(4, 325);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_FRONT_CENTER :
      servo_set_pwm(3, 310);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_RIGHT :
      servo_set_pwm(11, 300);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_LEFT :
      servo_set_pwm(10, 340);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_CENTER :
      servo_set_pwm(12, 245);
      vTaskDelay(250/portMAX_DELAY);
      break;
  }
}

void cocobot_arm_action_pliers_open(pliers_e pliers)
{
  switch (pliers)
  {
    case PLIERS_FRONT_RIGHT :
      servo_set_pwm(2, 170);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_FRONT_LEFT :
      servo_set_pwm(4, 280);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_FRONT_CENTER :
      servo_set_pwm(3, 355);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_RIGHT :
      servo_set_pwm(11, 340);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_LEFT :
      servo_set_pwm(10, 295);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_BACK_CENTER :
      servo_set_pwm(12, 285);
      vTaskDelay(250/portMAX_DELAY);
      break;
  }
}


void cocobot_arm_action_pliers_up(pliersCarrier_e pliersCarrier)
{
  switch (pliersCarrier)
  {
    case PLIERS_CARRIER_FRONT:
      servo_set_pwm(5, 220);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_CARRIER_BACK:
      servo_set_pwm(7, 200);
      vTaskDelay(250/portMAX_DELAY);
      break;
  }
}

void cocobot_arm_action_pliers_down(pliersCarrier_e pliersCarrier)
{
  switch (pliersCarrier)
  {
    case PLIERS_CARRIER_FRONT:
      servo_set_pwm(5, 440);
      vTaskDelay(250/portMAX_DELAY);
      break;
    case PLIERS_CARRIER_BACK:
      servo_set_pwm(7, 420);
      vTaskDelay(250/portMAX_DELAY);
      break;
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
      servo_set_angle(4, -90);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -110);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -90);
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
      servo_set_angle(3, 80);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -130);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(5, 70);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(4, -100);
      vTaskDelay(250/portTICK_PERIOD_MS);
      servo_set_angle(3, 65);
      vTaskDelay(250/portTICK_PERIOD_MS);
      pump_set_state(arm_id, 0);
      vTaskDelay(1500/portTICK_PERIOD_MS);
 
      break;
  }
}
