#include <cocobot.h>
#include <platform.h>

#define MECA_BEE_SERVO_ID   PLATFORM_SERVO_3_ID

#define MECA_BEE_SERVO_INIT     125

#define MECA_BEE_SERVO_CLOSE_GREEN    125
#define MECA_BEE_SERVO_OPEN_GREEN     500

#define MECA_BEE_SERVO_CLOSE_ORANGE    400
#define MECA_BEE_SERVO_OPEN_ORANGE     125


static unsigned int servo_set_point;

static void meca_bee_update(void)
{
  platform_servo_set_value(MECA_BEE_SERVO_ID, servo_set_point);
}

void meca_bee_init(void)
{
  servo_set_point = MECA_BEE_SERVO_INIT;

  meca_bee_update();
}

void meca_bee_action(void)
{
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_POS)
  {
    servo_set_point = MECA_BEE_SERVO_CLOSE_ORANGE;
  }
  else
  {
    servo_set_point = MECA_BEE_SERVO_CLOSE_GREEN;
  }

  meca_bee_update();
}

void meca_bee_prepare(void)
{
  if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_POS)
  {
    servo_set_point = MECA_BEE_SERVO_OPEN_ORANGE;
  }
  else
  {
    servo_set_point = MECA_BEE_SERVO_OPEN_GREEN;
  }

  meca_bee_update();
}

void com_bee_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  switch(pid)
  {
    case COCOBOT_COM_MECA_ACTION_PID:
      {
        uint8_t id;
        uint32_t offset = 0;
        offset += cocobot_com_read_B(data, len, offset, &id);

        switch(id)
        {
          case 0:
            meca_bee_init();
            break;

          case 1:
            meca_bee_prepare();
            break;

          case 2:
            meca_bee_action();
            break;
        }
      }
      break;
  }
}
