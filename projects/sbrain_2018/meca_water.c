#include <cocobot.h>
#include <platform.h>
#include "FreeRTOS.h"
#include "task.h"

#define MECA_WATER_SERVO_BALL_RIGHT_ID   PLATFORM_SERVO_1_ID
#define MECA_WATER_SERVO_BALL_RIGHT_OPEN      300
#define MECA_WATER_SERVO_BALL_RIGHT_CLOSE     150

#define MECA_WATER_SERVO_TAQUET_ID   PLATFORM_SERVO_0_ID
#define MECA_WATER_SERVO_TAQUET_OPEN    200
#define MECA_WATER_SERVO_TAQUET_CLOSE   430
#define MECA_WATER_SERVO_TAQUET_INITIATE   480

//Used for 
#define MECA_WATER_SERVO_TAQUET_SCALE_MIN  260
#define MECA_WATER_SERVO_TAQUET_SCALE_1  270
#define MECA_WATER_SERVO_TAQUET_SCALE_2  280
#define MECA_WATER_SERVO_TAQUET_SCALE_3  290
#define MECA_WATER_SERVO_TAQUET_SCALE_4  300
#define MECA_WATER_SERVO_TAQUET_MIDDLE  310
#define MECA_WATER_SERVO_TAQUET_SCALE_5  320
#define MECA_WATER_SERVO_TAQUET_SCALE_6  330
#define MECA_WATER_SERVO_TAQUET_SCALE_7  340
#define MECA_WATER_SERVO_TAQUET_SCALE_8  350
#define MECA_WATER_SERVO_TAQUET_SCALE_MAX  360

#define MECA_WATER_SERVO_ROTATION_ID    PLATFORM_SERVO_2_ID
#define MECA_WATER_SERVO_ROTATION_OPEN    350
#define MECA_WATER_SERVO_ROTATION_CLOSE   200


//static unsigned int servo_take_tube[11] = {260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360};
static unsigned int servo_rotation_set_point;
static unsigned int servo_ball_right_set_point;
static unsigned int servo_taquet_set_point;

static void meca_water_update(void)
{
  platform_servo_set_value(MECA_WATER_SERVO_BALL_RIGHT_ID, servo_ball_right_set_point);
  platform_servo_set_value(MECA_WATER_SERVO_ROTATION_ID, servo_rotation_set_point);
  platform_servo_set_value(MECA_WATER_SERVO_TAQUET_ID, servo_taquet_set_point);
}

//static void meca_water_run_goto_static_tube()
//{
//    for(int i = 11; i > 0; i--)
//    {
//        servo_taquet_set_point = servo_take_tube[i];
//        meca_water_update();
//        vTaskDelay(80/portTICK_PERIOD_MS);
//    } 
//}
//
//static void meca_water_run_goto_pivot_tube()
//{
//    for(int i = 0; i < 11; i++)
//    {
//        servo_taquet_set_point = servo_take_tube[i];
//        meca_water_update();
//        vTaskDelay(80/portTICK_PERIOD_MS);
//    } 
//}

void meca_water_init(void)
{
  servo_rotation_set_point = MECA_WATER_SERVO_ROTATION_CLOSE;
  servo_ball_right_set_point = MECA_WATER_SERVO_BALL_RIGHT_CLOSE;
  servo_taquet_set_point = MECA_WATER_SERVO_TAQUET_OPEN;
  meca_water_update();
}

void meca_water_prepare(void)
{
  servo_taquet_set_point = MECA_WATER_SERVO_TAQUET_OPEN;
  meca_water_update();
}

void meca_water_activate(void)
{
  servo_taquet_set_point = MECA_WATER_SERVO_TAQUET_INITIATE;
  meca_water_update();
}

void meca_water_take_from_distributor(void)
{
  servo_taquet_set_point = MECA_WATER_SERVO_TAQUET_CLOSE;
  meca_water_update();
  vTaskDelay(500/portTICK_PERIOD_MS);
  //for(int i = 0; i < 4; i++)
  //{
  //    meca_water_run_goto_static_tube();
  //    meca_water_run_goto_pivot_tube();
  //}
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

