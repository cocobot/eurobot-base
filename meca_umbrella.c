#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include "meca_umbrella.h"

typedef enum
{
  MECA_UMBRELLA_CLOSE,
  MECA_UMBRELLA_OPEN,
  MECA_UMBRELLA_DISABLE,
} meca_umbrella_state_t;


#define MECA_UMBRELLA_ROT_SERVO_ID         PLATFORM_SERVO_15_ID
#define MECA_UMBRELLA_OPN_SERVO_ID         PLATFORM_SERVO_15_ID

#define MECA_UMBRELLA_ROT_SERVO_CLOSE 0
#define MECA_UMBRELLA_ROT_SERVO_OPEN 0
#define MECA_UMBRELLA_ROT_SERVO_DISABLE 0

#define MECA_UMBRELLA_OPN_SERVO_CLOSE 0
#define MECA_UMBRELLA_OPN_SERVO_OPEN 0
#define MECA_UMBRELLA_OPN_SERVO_DISABLE 0

#define MECA_UMBRELLA_DELAY_MS 500

static meca_umbrella_state_t state;

static unsigned int servo_rot_set_point;
static unsigned int servo_opn_set_point;

void meca_umbrella_init(void)
{
  servo_rot_set_point = MECA_UMBRELLA_ROT_SERVO_DISABLE;
  servo_opn_set_point = MECA_UMBRELLA_OPN_SERVO_DISABLE;

  meca_umbrella_close();
}

static void meca_umbrella_update_servo(void)
{
  platform_servo_set_value(MECA_UMBRELLA_OPN_SERVO_ID, servo_opn_set_point);
  platform_servo_set_value(MECA_UMBRELLA_ROT_SERVO_ID, servo_rot_set_point);
}

void meca_umbrella_disable(void)
{
  servo_rot_set_point = MECA_UMBRELLA_ROT_SERVO_DISABLE;
  servo_opn_set_point = MECA_UMBRELLA_OPN_SERVO_DISABLE;

  state = MECA_UMBRELLA_DISABLE;

  meca_umbrella_update_servo();
}

void meca_umbrella_close(void)
{
  state = MECA_UMBRELLA_CLOSE;

  servo_opn_set_point = MECA_UMBRELLA_OPN_SERVO_CLOSE;
  meca_umbrella_update_servo();

  vTaskDelay(MECA_UMBRELLA_DELAY_MS / portTICK_PERIOD_MS);

  servo_rot_set_point = MECA_UMBRELLA_ROT_SERVO_CLOSE;
  meca_umbrella_update_servo();
}

void meca_umbrella_open(void)
{
  state = MECA_UMBRELLA_CLOSE;

  servo_rot_set_point = MECA_UMBRELLA_ROT_SERVO_OPEN;
  meca_umbrella_update_servo();

  vTaskDelay(MECA_UMBRELLA_DELAY_MS / portTICK_PERIOD_MS);

  servo_opn_set_point = MECA_UMBRELLA_OPN_SERVO_OPEN;
  meca_umbrella_update_servo();
}

int meca_umbrella_console_handler(const char * command)
{
  if(strcmp(command,"meca_umbrella") == 0)
  {
    char buf[16];
    if(cocobot_console_get_sargument(0, buf, sizeof(buf)))
    {
      if(strcmp(buf, "open") == 0)
      {
        meca_umbrella_open();
      }
      if(strcmp(buf, "close") == 0)
      {
        meca_umbrella_close();
      }
      if(strcmp(buf, "disable") == 0)
      {
        meca_umbrella_disable();
      }
    }

    switch(state)
    {
      case MECA_UMBRELLA_OPEN:
        cocobot_console_send_answer("open");
        break;

      case MECA_UMBRELLA_CLOSE:
        cocobot_console_send_answer("close");
        break;

      case MECA_UMBRELLA_DISABLE:
        cocobot_console_send_answer("disable");
        break;
    }

    return 1;
  }

  if(strcmp(command,"meca_umbrella_rot_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_rot_set_point = set;
      meca_umbrella_update_servo();
    }
    cocobot_console_send_answer("%u", servo_rot_set_point);
    return 1;
  }

  if(strcmp(command,"meca_umbrella_opn_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_opn_set_point = set;
      meca_umbrella_update_servo();
    }
    cocobot_console_send_answer("%u", servo_opn_set_point);
    return 1;
  }

  return 0;
}
