#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include "meca_seashell.h"

#define MECA_SEASHELL_LEFT_SERVO_ID   PLATFORM_SERVO_12_ID
#define MECA_SEASHELL_RIGHT_SERVO_ID  PLATFORM_SERVO_11_ID

#define MECA_SEASHELL_LEFT_OPEN  230
#define MECA_SEASHELL_LEFT_CLOSE   420
#define MECA_SEASHELL_RIGHT_OPEN  430
#define MECA_SEASHELL_RIGHT_CLOSE   250

typedef enum
{
  MECA_SEASHELL_OPEN,
  MECA_SEASHELL_CLOSE,
} meca_seashell_state_t;

static meca_seashell_state_t state;
static unsigned int servo_left_set_point;
static unsigned int servo_right_set_point;

static void meca_seashell_update(void)
{
  platform_servo_set_value(MECA_SEASHELL_LEFT_SERVO_ID, servo_left_set_point);
  platform_servo_set_value(MECA_SEASHELL_RIGHT_SERVO_ID, servo_right_set_point);
}

void meca_seashell_init(void)
{
  meca_seashell_close();
}

void meca_seashell_close(void)
{
  state = MECA_SEASHELL_CLOSE;

  servo_left_set_point = MECA_SEASHELL_LEFT_CLOSE;
  servo_right_set_point = MECA_SEASHELL_RIGHT_CLOSE;

  meca_seashell_update();
}

void meca_seashell_open(void)
{
  state = MECA_SEASHELL_OPEN;

  servo_left_set_point = MECA_SEASHELL_LEFT_OPEN;
  servo_right_set_point = MECA_SEASHELL_RIGHT_OPEN;

  meca_seashell_update();
}

int meca_seashell_console_handler(const char * command)
{
  if(strcmp(command,"meca_seashell") == 0)
  {
    char buf[16];
    if(cocobot_console_get_sargument(0, buf, sizeof(buf)))
    {
      if(strcmp(buf, "open") == 0)
      {
        meca_seashell_open();
      }
      if(strcmp(buf, "close") == 0)
      {
        meca_seashell_close();
      }
    }

    switch(state)
    {
      case MECA_SEASHELL_OPEN:
        cocobot_console_send_answer("open");
        break;

      case MECA_SEASHELL_CLOSE:
        cocobot_console_send_answer("close");
        break;
    }

    return 1;
  }

  if(strcmp(command,"meca_seashell_left_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_left_set_point = set;
      meca_seashell_update();
    }
    cocobot_console_send_answer("%u", servo_left_set_point);
    return 1;
  }

  if(strcmp(command,"meca_seashell_right_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_right_set_point = set;
      meca_seashell_update();
    }
    cocobot_console_send_answer("%u", servo_right_set_point);
    return 1;
  }

  return 0;
}
