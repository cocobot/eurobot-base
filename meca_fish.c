#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include "meca_fish.h"

#define MECA_FISH_LR_SERVO_ID         PLATFORM_SERVO_2_ID
#define MECA_FISH_UD_SERVO_ID         PLATFORM_SERVO_3_ID
#define MECA_FISH_ROT_SERVO_ID        PLATFORM_SERVO_4_ID

#define MECA_FISH_LR_LEFT    1300
#define MECA_FISH_LR_CENTER  1400
#define MECA_FISH_LR_RIGHT   1500

#define MECA_FISH_ROT_UP     1500
#define MECA_FISH_ROT_DOWN   1600

#define MECA_FISH_UD_UP      1500
#define MECA_FISH_UD_DOWN    1400

typedef enum
{
  MECA_FISH_CLOSE,
  MECA_FISH_READY,
} meca_fish_state_t;

static meca_fish_state_t state;
static unsigned int servo_lr_set_point;
static unsigned int servo_ud_set_point;
static unsigned int servo_rot_set_point;

static void meca_fish_update(void)
{
  platform_servo_set_value(MECA_FISH_UD_SERVO_ID, servo_ud_set_point);
  platform_servo_set_value(MECA_FISH_ROT_SERVO_ID, servo_rot_set_point);
  platform_servo_set_value(MECA_FISH_LR_SERVO_ID, servo_lr_set_point);
}

void meca_fish_close(void)
{
  state = MECA_FISH_CLOSE;

  servo_lr_set_point = MECA_FISH_LR_CENTER;
  servo_ud_set_point = MECA_FISH_UD_DOWN;
  servo_rot_set_point = MECA_FISH_ROT_UP;

  meca_fish_update();
}

void meca_fish_ready(void)
{
  state = MECA_FISH_READY;

  servo_lr_set_point = MECA_FISH_LR_CENTER;
  servo_ud_set_point = MECA_FISH_UD_DOWN;
  servo_rot_set_point = MECA_FISH_ROT_DOWN;

  meca_fish_update();
}

void meca_fish_init(void)
{
  meca_fish_close();
}

int meca_fish_console_handler(const char * command)
{
  if(strcmp(command,"meca_fish") == 0)
  {
    char buf[16];
    if(cocobot_console_get_sargument(0, buf, sizeof(buf)))
    {
      if(strcmp(buf, "close") == 0)
      {
        meca_fish_close();
      }
      if(strcmp(buf, "ready") == 0)
      {
        meca_fish_ready();
      }
    }

    switch(state)
    {
      case MECA_FISH_CLOSE:
        cocobot_console_send_answer("close");
        break;

      case MECA_FISH_READY:
        cocobot_console_send_answer("ready");
        break;
    }

    return 1;
  }

  if(strcmp(command,"meca_fish_ud_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_ud_set_point = set;
      meca_fish_update();
    }
    cocobot_console_send_answer("%u", servo_ud_set_point);
    return 1;
  }

  if(strcmp(command,"meca_fish_lr_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_lr_set_point = set;
      meca_fish_update();
    }
    cocobot_console_send_answer("%u", servo_lr_set_point);
    return 1;
  }

  if(strcmp(command,"meca_fish_rot_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_rot_set_point = set;
      meca_fish_update();
    }
    cocobot_console_send_answer("%u", servo_rot_set_point);
    return 1;
  }

  return 0;
}
