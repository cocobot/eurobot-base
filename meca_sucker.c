#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include "meca_sucker.h"

void meca_sucker_init(void)
{
  platform_gpio_set_direction(PLATFORM_GPIO0, MCUAL_GPIO_OUTPUT);

  meca_sucker_set_state(MECA_SUCKER_LEFT, MECA_SUCKER_CLOSE);
  meca_sucker_set_state(MECA_SUCKER_RIGHT, MECA_SUCKER_CLOSE);
}

void meca_sucker_set_state(meca_sucker_id_t id, meca_sucker_state_t state)
{
  (void)id;

  switch(state)
  {
    case MECA_SUCKER_CLOSE:
      platform_gpio_clear(PLATFORM_GPIO0);
      break;

    case MECA_SUCKER_OPEN:
      break;

    case MECA_SUCKER_PUMP:
      platform_gpio_set(PLATFORM_GPIO0);
      break;
  }
}
