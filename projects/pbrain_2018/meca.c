#include <cocobot.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>

#define MECA_SERVO_CLAW_RIGHT_ID   PLATFORM_SERVO_0_ID
#define MECA_SERVO_CLAW_RIGHT_INIT      190
#define MECA_SERVO_CLAW_RIGHT_OPEN      325
#define MECA_SERVO_CLAW_RIGHT_CLOSE     375

#define MECA_SERVO_HOR_RIGHT_ID   PLATFORM_SERVO_1_ID
#define MECA_SERVO_HOR_RIGHT_INIT      175
#define MECA_SERVO_HOR_RIGHT_OPEN      375

#define MECA_SERVO_CLAW_LEFT_ID   PLATFORM_SERVO_2_ID
#define MECA_SERVO_CLAW_LEFT_INIT      275
#define MECA_SERVO_CLAW_LEFT_OPEN      350
#define MECA_SERVO_CLAW_LEFT_CLOSE     400

#define MECA_SERVO_HOR_LEFT_ID   PLATFORM_SERVO_3_ID
#define MECA_SERVO_HOR_LEFT_INIT      450
#define MECA_SERVO_HOR_LEFT_OPEN      270

static unsigned int servo_claw_right_set_point;
static unsigned int servo_claw_left_set_point;
static unsigned int servo_hor_right_set_point;
static unsigned int servo_hor_left_set_point;

static void meca_update(void)
{
  platform_servo_set_value(MECA_SERVO_CLAW_LEFT_ID, servo_claw_left_set_point);
  platform_servo_set_value(MECA_SERVO_CLAW_RIGHT_ID, servo_claw_right_set_point);
  platform_servo_set_value(MECA_SERVO_HOR_LEFT_ID, servo_hor_left_set_point);
  platform_servo_set_value(MECA_SERVO_HOR_RIGHT_ID, servo_hor_right_set_point);
}

void meca_wait_busy(void)
{
  while(1)
  {
    uint8_t ret = 0;
    platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
    ret = platform_spi_slave_transfert(0x00);
    platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);

    if(ret == 0)
    {
       return;
    }
  }
}

void meca_taquet_left_left(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x12);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_left_center_get(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x22);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_left_center_push(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x32);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_left_right(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x42);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}


void meca_taquet_right_right(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x13);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_right_center_get(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x23);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_right_center_push(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x33);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_taquet_right_left(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x43);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}



void meca_claw_right_up(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x20);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_claw_left_up(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x21);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_claw_right_down(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x10);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}

void meca_claw_left_down(void)
{
  platform_spi_slave_select(PLATFORM_SPI_CS0_SELECT);
  platform_spi_slave_transfert(0x11);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}



void meca_init(void)
{

  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_INIT;
  meca_update();
  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_INIT;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);
  servo_claw_left_set_point = 0;
  servo_claw_right_set_point = 0;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_INIT;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_INIT;
  meca_update();



  /*
  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;
  meca_update();


  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_CLOSE;
  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_CLOSE;
  meca_update();

  meca_claw_left_down();
  meca_claw_right_down();
  return;

  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_INIT;
  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_INIT;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);

  meca_claw_right_down();
  meca_claw_left_down();
  meca_wait_busy();


  meca_claw_right_up();
  meca_claw_left_up();
  meca_wait_busy();

  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_INIT;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_INIT;
  meca_update();

  meca_taquet_left_left();
  meca_taquet_right_right();
  meca_wait_busy();
  */
}

void meca_prepare(void)
{
  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_INIT;
  meca_update();
  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_INIT;
  meca_update();

  vTaskDelay(500 / portTICK_PERIOD_MS);

  vTaskDelay(500 / portTICK_PERIOD_MS);
  servo_claw_left_set_point = 0;
  servo_claw_right_set_point = 0;
  meca_update();



  //servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  //servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;
  //meca_update();

  /*
  vTaskDelay(250 / portTICK_PERIOD_MS);

  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_OPEN;
  meca_update();

  vTaskDelay(250 / portTICK_PERIOD_MS);

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_OPEN;
  meca_update();
  */
}

void meca_take(void)
{
  servo_hor_right_set_point = MECA_SERVO_HOR_RIGHT_OPEN;
  servo_hor_left_set_point = MECA_SERVO_HOR_LEFT_OPEN;
  meca_update();
  vTaskDelay(500 / portTICK_PERIOD_MS);

  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_CLOSE;
  meca_update();

  vTaskDelay(250 / portTICK_PERIOD_MS);

  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_CLOSE;
  meca_update();
}

void meca_take_left(void)
{
  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_CLOSE;
  meca_update();
}

void meca_release_left(void)
{
  servo_claw_left_set_point = MECA_SERVO_CLAW_LEFT_OPEN;
  meca_update();
}

void meca_take_right(void)
{
  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_CLOSE;
  meca_update();
}

void meca_release_right(void)
{
  servo_claw_right_set_point = MECA_SERVO_CLAW_RIGHT_OPEN;
  meca_update();
}

void meca_chore_1(void)
{
  meca_taquet_left_left();
  meca_taquet_right_left();
  meca_wait_busy();

  cocobot_trajectory_goto_d(30, 1000);
  cocobot_trajectory_wait();

  meca_taquet_left_right();
  meca_taquet_right_right();
  meca_wait_busy();

}

void meca_com_handler(uint16_t pid, uint8_t * data, uint32_t len)
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
          case 0x80:
            meca_taquet_left_left();
            break;

          case 0x81:
            meca_taquet_left_center_get();
            break;

          case 0x82:
            meca_taquet_left_center_push();
            break;

          case 0x83:
            meca_taquet_left_right();
            break;

          case 0x84:
            meca_taquet_right_right();
            break;

          case 0x85:
            meca_taquet_right_center_get();
            break;

          case 0x86:
            meca_taquet_right_center_push();
            break;

          case 0x87:
            meca_taquet_right_left();
            break;

          case 0x88:
            meca_claw_right_up();
            break;

          case 0x89:
            meca_claw_right_down();
            break;

          case 0x8a:
            meca_claw_left_up();
            break;

          case 0x8b:
            meca_claw_left_down();
            break;

          case 0x8c:
            meca_init();
            break;

          case 0x8d:
            meca_prepare();
            break;

          case 0x8e:
            meca_take_left();
            break;

          case 0x8f:
            meca_release_left();
            break;

          case 0x90:
            meca_take_right();
            break;

          case 0x91:
            meca_release_right();
            break;

          case 0xC0:
            meca_chore_1();
            break;
        }
      }
      break;
  }
}
