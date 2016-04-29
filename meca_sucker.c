#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include "meca_sucker.h"

#define MECA_SUCKER_LEFT_PUMP   PLATFORM_GPIO3
#define MECA_SUCKER_LEFT_SUCKER PLATFORM_GPIO2
#define MECA_SUCKER_RIGHT_PUMP   PLATFORM_GPIO5
#define MECA_SUCKER_RIGHT_SUCKER PLATFORM_GPIO4

#define MECA_SUCKER_LEFT_SERVO_ID         PLATFORM_SERVO_0_ID
#define MECA_SUCKER_RIGHT_SERVO_ID         PLATFORM_SERVO_2_ID

#define MECA_SUCKER_LEFT_SERVO_CLOSE 350
#define MECA_SUCKER_LEFT_SERVO_OPEN 225
#define MECA_SUCKER_LEFT_SERVO_DISABLE 0

#define MECA_SUCKER_RIGHT_SERVO_CLOSE 370
#define MECA_SUCKER_RIGHT_SERVO_OPEN 500
#define MECA_SUCKER_RIGHT_SERVO_DISABLE 0

static meca_sucker_state_t left_state;
static meca_sucker_state_t right_state;

static unsigned int servo_left_set_point;
static unsigned int servo_right_set_point;

void meca_sucker_init(void)
{
  platform_gpio_set_direction(MECA_SUCKER_LEFT_PUMP, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_LEFT_SUCKER, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_RIGHT_PUMP, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(MECA_SUCKER_RIGHT_SUCKER, MCUAL_GPIO_OUTPUT);

  servo_left_set_point = MECA_SUCKER_LEFT_SERVO_DISABLE;
  servo_right_set_point = MECA_SUCKER_RIGHT_SERVO_DISABLE;

  meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_CLOSE);
  meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_CLOSE);
}

void meca_sucker_disable(void)
{
  meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_DISABLE);
  meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_DISABLE);
}

static void meca_sucker_update_servo(void)
{
  platform_servo_set_value(MECA_SUCKER_LEFT_SERVO_ID, servo_left_set_point);
  platform_servo_set_value(MECA_SUCKER_RIGHT_SERVO_ID, servo_right_set_point);
}

void meca_sucker_set_state(meca_sucker_id_t id, meca_sucker_state_t state)
{
  switch(id)
  {
    case MECA_SUCKER_LEFT:
      left_state = state;
      switch(state)
      {
        case MECA_SUCKER_CLOSE:
          platform_gpio_clear(MECA_SUCKER_LEFT_PUMP);
          platform_gpio_clear(MECA_SUCKER_LEFT_SUCKER);
          servo_left_set_point = MECA_SUCKER_LEFT_SERVO_CLOSE;
          break;

        case MECA_SUCKER_OPEN:
          platform_gpio_clear(MECA_SUCKER_LEFT_PUMP);
          platform_gpio_set(MECA_SUCKER_LEFT_SUCKER);
          servo_left_set_point = MECA_SUCKER_LEFT_SERVO_OPEN;
          break;

        case MECA_SUCKER_PUMP:
          platform_gpio_set(MECA_SUCKER_LEFT_PUMP);
          platform_gpio_clear(MECA_SUCKER_LEFT_SUCKER);
          servo_left_set_point = MECA_SUCKER_LEFT_SERVO_OPEN;
          break;

        case MECA_SUCKER_DISABLE:
          platform_gpio_clear(MECA_SUCKER_LEFT_PUMP);
          platform_gpio_clear(MECA_SUCKER_LEFT_PUMP);
          servo_left_set_point = MECA_SUCKER_LEFT_SERVO_DISABLE;
          break;
      }
      break;

    case MECA_SUCKER_RIGHT:
      right_state = state;
      switch(state)
      {
        case MECA_SUCKER_CLOSE:
          platform_gpio_clear(MECA_SUCKER_RIGHT_PUMP);
          platform_gpio_clear(MECA_SUCKER_RIGHT_SUCKER);
          servo_right_set_point = MECA_SUCKER_RIGHT_SERVO_CLOSE;
          break;

        case MECA_SUCKER_OPEN:
          platform_gpio_clear(MECA_SUCKER_RIGHT_PUMP);
          platform_gpio_set(MECA_SUCKER_RIGHT_SUCKER);
          servo_right_set_point = MECA_SUCKER_RIGHT_SERVO_OPEN;
          break;

        case MECA_SUCKER_PUMP:
          platform_gpio_set(MECA_SUCKER_RIGHT_PUMP);
          platform_gpio_clear(MECA_SUCKER_RIGHT_SUCKER);
          servo_right_set_point = MECA_SUCKER_RIGHT_SERVO_OPEN;
          break;

        case MECA_SUCKER_DISABLE:
          platform_gpio_clear(MECA_SUCKER_RIGHT_PUMP);
          platform_gpio_clear(MECA_SUCKER_RIGHT_PUMP);
          servo_right_set_point = MECA_SUCKER_RIGHT_SERVO_DISABLE;
          break;
      }
      break;
  }

  meca_sucker_update_servo();
}

int meca_sucker_console_handler(const char * command)
{
  if(strcmp(command,"meca_sucker_left") == 0)
  {
    char buf[16];
    if(cocobot_console_get_sargument(0, buf, sizeof(buf)))
    {
      if(strcmp(buf, "open") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_OPEN);
      }
      if(strcmp(buf, "close") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_CLOSE);
      }
      if(strcmp(buf, "pump") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_PUMP);
      }
      if(strcmp(buf, "disable") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_DISABLE);
      }
    }

    switch(left_state)
    {
      case MECA_SUCKER_OPEN:
        cocobot_console_send_answer("open");
        break;

      case MECA_SUCKER_CLOSE:
        cocobot_console_send_answer("close");
        break;

      case MECA_SUCKER_PUMP:
        cocobot_console_send_answer("pump");
        break;

      case MECA_SUCKER_DISABLE:
        cocobot_console_send_answer("disable");
        break;
    }

    return 1;
  }

  if(strcmp(command,"meca_sucker_right") == 0)
  {
    char buf[16];
    if(cocobot_console_get_sargument(0, buf, sizeof(buf)))
    {
      if(strcmp(buf, "open") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_OPEN);
      }
      if(strcmp(buf, "close") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_CLOSE);
      }
      if(strcmp(buf, "pump") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_PUMP);
      }
      if(strcmp(buf, "disable") == 0)
      {
        meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_DISABLE);
      }
    }

    switch(right_state)
    {
      case MECA_SUCKER_OPEN:
        cocobot_console_send_answer("open");
        break;

      case MECA_SUCKER_CLOSE:
        cocobot_console_send_answer("close");
        break;

      case MECA_SUCKER_PUMP:
        cocobot_console_send_answer("pump");
        break;

      case MECA_SUCKER_DISABLE:
        cocobot_console_send_answer("disable");
        break;
    }

    return 1;
  }

  if(strcmp(command,"meca_sucker_left_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_left_set_point = set;
      meca_sucker_update_servo();
    }
    cocobot_console_send_answer("%u", servo_left_set_point);
    return 1;
  }

  if(strcmp(command,"meca_sucker_right_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_right_set_point = set;
      meca_sucker_update_servo();
    }
    cocobot_console_send_answer("%u", servo_right_set_point);
    return 1;
  }

  return 0;
}
