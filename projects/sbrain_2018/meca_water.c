#include <cocobot.h>
#include <platform.h>

#define MECA_WATER_SERVO_BALL_RIGHT_ID   PLATFORM_SERVO_1_ID
#define MECA_WATER_SERVO_BALL_RIGHT_OPEN      300
#define MECA_WATER_SERVO_BALL_RIGHT_CLOSE     150

#define MECA_WATER_SERVO_TAQUET_ID   PLATFORM_SERVO_0_ID

#define MECA_WATER_SERVO_ROTATION_ID    PLATFORM_SERVO_2_ID
#define MECA_WATER_SERVO_ROTATION_OPEN    350
#define MECA_WATER_SERVO_ROTATION_CLOSE   200


static unsigned int servo_rotation_set_point;
static unsigned int servo_ball_right_set_point;

static void meca_water_update(void)
{
  platform_servo_set_value(MECA_WATER_SERVO_BALL_RIGHT_ID, servo_ball_right_set_point);
  platform_servo_set_value(MECA_WATER_SERVO_ROTATION_ID, servo_rotation_set_point);
}

void meca_water_init(void)
{
  servo_rotation_set_point = MECA_WATER_SERVO_ROTATION_CLOSE;
  servo_ball_right_set_point = MECA_WATER_SERVO_BALL_RIGHT_CLOSE;
  meca_water_update();
}

void meca_water_prepare(void)
{
  //servo_set_point = MECA_BEE_SERVO_INIT;
  meca_water_update();
}

void meca_water_take_from_distributor(void)
{
  //servo_set_point = MECA_BEE_SERVO_INIT;
  meca_water_update();
}

void meca_water_shoot_left(void)
{
  //servo_set_point = MECA_BEE_SERVO_INIT;
  meca_water_update();
}

void meca_water_shoot_all(void)
{
  //set servo
  //servo_set_point = MECA_BEE_SERVO_INIT;
  meca_water_update();

  //turn motor on

  //turn motor off
}

void meca_water_release_bad_water(void)
{
  //servo_set_point = MECA_BEE_SERVO_INIT;
  meca_water_update();
}

void com_water_handler(uint16_t pid, uint8_t * data, uint32_t len)
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
          case 3:
            meca_water_init();
            break;

          case 4:
            meca_water_prepare();
            break;

          case 5:
            meca_water_take_from_distributor();
            break;

          case 6:
            meca_water_shoot_left();
            break;

          case 7:
            meca_water_shoot_all();
            break;

          case 8:
            meca_water_release_bad_water();
            break;

        }
      }
      break;
  }
}

