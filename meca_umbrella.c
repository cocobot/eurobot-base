#include <string.h>
#include <cocobot.h>
#include "meca_umbrella.h"

typedef enum
{
  MECA_UMBRELLA_OPEN,
  MECA_UMBRELLA_CLOSE,
} meca_umbrella_state_t;


static meca_umbrella_state_t state;

void meca_umbrella_init(void)
{
  meca_umbrella_close();
}

void meca_umbrella_close(void)
{
  state = MECA_UMBRELLA_CLOSE;
}

void meca_umbrella_open(void)
{
  state = MECA_UMBRELLA_OPEN;
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
    }

    switch(state)
    {
      case MECA_UMBRELLA_OPEN:
        cocobot_console_send_answer("open");
        break;

      case MECA_UMBRELLA_CLOSE:
        cocobot_console_send_answer("close");
        break;
    }

    return 1;
  }
  return 0;
}
