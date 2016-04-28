#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include "meca_sucker.h"

static meca_sucker_state_t left_state;
static meca_sucker_state_t right_state;

void meca_sucker_init(void)
{
  platform_gpio_set_direction(PLATFORM_GPIO0, MCUAL_GPIO_OUTPUT);

  meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_CLOSE);
  meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_CLOSE);

  platform_gpio_set_direction(PLATFORM_GPIO2, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO3, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO4, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO5, MCUAL_GPIO_OUTPUT);
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
          platform_gpio_clear(PLATFORM_GPIO2);
          platform_gpio_clear(PLATFORM_GPIO3);
          break;

        case MECA_SUCKER_OPEN:
          platform_gpio_set(PLATFORM_GPIO2);
          platform_gpio_clear(PLATFORM_GPIO3);
          break;

        case MECA_SUCKER_PUMP:
          platform_gpio_clear(PLATFORM_GPIO2);
          platform_gpio_set(PLATFORM_GPIO3);
          break;
      }
      break;

    case MECA_SUCKER_RIGHT:
      right_state = state;
      switch(state)
      {
        case MECA_SUCKER_CLOSE:
          platform_gpio_clear(PLATFORM_GPIO4);
          platform_gpio_clear(PLATFORM_GPIO5);
          break;

        case MECA_SUCKER_OPEN:
          platform_gpio_set(PLATFORM_GPIO4);
          platform_gpio_clear(PLATFORM_GPIO5);
          break;

        case MECA_SUCKER_PUMP:
          platform_gpio_clear(PLATFORM_GPIO4);
          platform_gpio_set(PLATFORM_GPIO5);
          break;
      }
      break;
  }
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
    }

    return 1;
  }
  return 0;
}
