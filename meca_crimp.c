#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include "meca_crimp.h"

#define MECA_CRIMP_MAX_OPENNING 100

#define MECA_CRIMP_LEFT_SERVO_ID         PLATFORM_SERVO_15_ID
#define MECA_CRIMP_RIGHT_SERVO_ID        PLATFORM_SERVO_15_ID
#define MECA_CRIMP_VERTICAL_SERVO_ID     PLATFORM_SERVO_15_ID

#define MECA_CRIMP_LEFT_SERVO_OPEN 0
#define MECA_CRIMP_LEFT_SERVO_CLOSE 0
#define MECA_CRIMP_LEFT_SERVO_DISABLE 0

#define MECA_CRIMP_RIGHT_SERVO_OPEN 0
#define MECA_CRIMP_RIGHT_SERVO_CLOSE 0
#define MECA_CRIMP_RIGHT_SERVO_DISABLE 0

#define MECA_CRIMP_VERTICAL_SERVO_GROUND 0
#define MECA_CRIMP_VERTICAL_SERVO_FENCE 0
#define MECA_CRIMP_VERTICAL_SERVO_F1 0
#define MECA_CRIMP_VERTICAL_SERVO_DISABLE 0

static unsigned int servo_left_set_point;
static unsigned int servo_right_set_point;
static unsigned int servo_vertical_set_point;

static volatile int crimp_disable;
static volatile int crimp_current;
static volatile int crimp_target;

static void meca_crimp_update_servo(void)
{
  platform_servo_set_value(MECA_CRIMP_LEFT_SERVO_ID, servo_left_set_point);
  platform_servo_set_value(MECA_CRIMP_RIGHT_SERVO_ID, servo_right_set_point);
  platform_servo_set_value(MECA_CRIMP_VERTICAL_SERVO_ID, servo_vertical_set_point);
}

static void meca_crimp_task(void * arg)
{
  (void)arg;
  while(1)
  {
    crimp_current = platform_adc_get_mV(PLATFORM_ADC_CH0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void meca_crimp_init(void)
{
  crimp_disable = 1;

  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_DISABLE;
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_DISABLE;
  servo_vertical_set_point = MECA_CRIMP_VERTICAL_SERVO_DISABLE;

  xTaskCreate(meca_crimp_task, "crimp", 200, NULL, 2, NULL);
}

void meca_crimp_disable(void)
{
  crimp_target = MECA_CRIMP_MAX_OPENNING;
  crimp_disable = 4;

  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_DISABLE;
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_DISABLE;
  servo_vertical_set_point = MECA_CRIMP_VERTICAL_SERVO_DISABLE;
  meca_crimp_update_servo();
}

int meca_crimp_console_handler(const char * command)
{
  if(strcmp(command,"meca_crimp_left_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_left_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_left_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_right_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_right_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_right_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_vertical_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_vertical_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_vertical_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_target") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      crimp_target = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", crimp_target);
    return 1;
  }

  if(strcmp(command,"meca_crimp_current") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      crimp_current = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", crimp_current);
    return 1;
  }

  return 0;
}
