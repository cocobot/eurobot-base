#include <stdlib.h>
#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include "meca_fish.h"

#define MECA_FISH_SWEEP_STEP 10
#define MECA_FISH_SWEEP_DURATION_MS 1500

#define MECA_FISH_LR_SERVO_ID         PLATFORM_SERVO_14_ID
#define MECA_FISH_UD_SERVO_ID         PLATFORM_SERVO_0_ID
#define MECA_FISH_ROT_SERVO_ID        PLATFORM_SERVO_13_ID

#define MECA_FISH_LR_LEFT    250
#define MECA_FISH_LR_CENTER  290
#define MECA_FISH_LR_RIGHT   420
#define MECA_FISH_LR_DISABLE 0

#define MECA_FISH_ROT_UP          200
//#define MECA_FISH_ROT_HORIZONTAL  470
#define MECA_FISH_ROT_HORIZONTAL  490
#define MECA_FISH_ROT_DOWN        540
#define MECA_FISH_ROT_CHECK       380
#define MECA_FISH_ROT_DISABLE     0

#define MECA_FISH_UD_CHECK   370
#define MECA_FISH_UD_UP      340
#define MECA_FISH_UD_DOWN    300
#define MECA_FISH_UD_DISABLE 0

typedef enum
{
  MECA_FISH_CLOSE,
  MECA_FISH_PREPARE,
  MECA_FISH_WALK,
  MECA_FISH_SWEEP_HIGH_LEFT,
  MECA_FISH_SWEEP_HIGH_RIGHT,
  MECA_FISH_SWEEP_LEFT,
  MECA_FISH_SWEEP_RIGHT,
  MECA_FISH_SWIM_LEFT,
  MECA_FISH_SWIM_RIGHT,
  MECA_FISH_MANUAL,
  MECA_FISH_CHECK,
} meca_fish_state_t;

static volatile meca_fish_state_t current_state;
static volatile meca_fish_state_t next_state;
static EventGroupHandle_t busy;

static unsigned int servo_lr_set_point;
static unsigned int servo_ud_set_point;
static unsigned int servo_rot_set_point;

static unsigned int ir_value;
static unsigned int disabled;

static void meca_fish_update(void)
{
  platform_servo_set_value(MECA_FISH_UD_SERVO_ID, servo_ud_set_point);
  platform_servo_set_value(MECA_FISH_ROT_SERVO_ID, servo_rot_set_point);
  platform_servo_set_value(MECA_FISH_LR_SERVO_ID, servo_lr_set_point);
}

void meca_fish_task(void * arg)
{
  (void)arg;

  while(1)
  {
    if((next_state != current_state) && (!disabled))
    {
      current_state = next_state;

      switch(current_state)
      {
        case MECA_FISH_CLOSE:
          {
            servo_rot_set_point = MECA_FISH_ROT_DISABLE;
            servo_lr_set_point = MECA_FISH_LR_CENTER;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(500 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_UP;
            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
            vTaskDelay(250 / portTICK_PERIOD_MS);
          }
          break;

        case MECA_FISH_PREPARE:
          {
            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_CENTER;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;

        case MECA_FISH_WALK:
          {
            servo_rot_set_point = 320;
            servo_lr_set_point = MECA_FISH_LR_CENTER;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;

        case MECA_FISH_SWEEP_LEFT:
          {
            servo_rot_set_point = 320;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            int i;
            for(i = 0; i < MECA_FISH_SWEEP_STEP; i += 1)
            {
              servo_lr_set_point = MECA_FISH_LR_RIGHT + i * ((MECA_FISH_LR_LEFT - MECA_FISH_LR_RIGHT)) / (MECA_FISH_SWEEP_STEP - 1);
              meca_fish_update();
              vTaskDelay((MECA_FISH_SWEEP_DURATION_MS / MECA_FISH_SWEEP_STEP) / portTICK_PERIOD_MS);
            }

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;
          
        case MECA_FISH_SWEEP_RIGHT:
          {
            servo_rot_set_point = 320;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            int i;
            for(i = 0; i < MECA_FISH_SWEEP_STEP; i += 1)
            {
              servo_lr_set_point = MECA_FISH_LR_LEFT + i * ((MECA_FISH_LR_RIGHT - MECA_FISH_LR_LEFT)) / (MECA_FISH_SWEEP_STEP - 1);
              meca_fish_update();
              vTaskDelay((MECA_FISH_SWEEP_DURATION_MS / MECA_FISH_SWEEP_STEP) / portTICK_PERIOD_MS);
            }

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;

        case MECA_FISH_SWEEP_HIGH_LEFT:
          {
            servo_rot_set_point = 320;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = 470;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            int i;
            for(i = 0; i < MECA_FISH_SWEEP_STEP; i += 1)
            {
              servo_lr_set_point = MECA_FISH_LR_RIGHT + i * ((MECA_FISH_LR_LEFT - MECA_FISH_LR_RIGHT)) / (MECA_FISH_SWEEP_STEP - 1);
              meca_fish_update();
              vTaskDelay((MECA_FISH_SWEEP_DURATION_MS / MECA_FISH_SWEEP_STEP) / portTICK_PERIOD_MS);
            }

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;
          
        case MECA_FISH_SWEEP_HIGH_RIGHT:
          {
            servo_rot_set_point = 320;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = 470;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            int i;
            for(i = 0; i < MECA_FISH_SWEEP_STEP; i += 1)
            {
              servo_lr_set_point = MECA_FISH_LR_LEFT + i * ((MECA_FISH_LR_RIGHT - MECA_FISH_LR_LEFT)) / (MECA_FISH_SWEEP_STEP - 1);
              meca_fish_update();
              vTaskDelay((MECA_FISH_SWEEP_DURATION_MS / MECA_FISH_SWEEP_STEP) / portTICK_PERIOD_MS);
            }

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;

        case MECA_FISH_SWIM_LEFT:
          {
            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(500 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_DOWN;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_DOWN;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();

            next_state = MECA_FISH_MANUAL;
          }
          break;

        case MECA_FISH_SWIM_RIGHT:
          {
            next_state = MECA_FISH_MANUAL;

            int i;
            for(i = 0; i < 5; i += 1)
            {
            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_DOWN;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_DOWN;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_RIGHT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_LEFT;
            servo_ud_set_point = MECA_FISH_UD_UP;
            meca_fish_update();
            vTaskDelay(200 / portTICK_PERIOD_MS);

            }

            servo_rot_set_point = MECA_FISH_ROT_HORIZONTAL;
            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();

          }
          break;

        case MECA_FISH_CHECK:
          {
            next_state = MECA_FISH_MANUAL;

            servo_rot_set_point = MECA_FISH_ROT_CHECK;
            servo_lr_set_point = MECA_FISH_LR_CENTER;
            servo_ud_set_point = MECA_FISH_UD_DOWN;
            meca_fish_update();

            //servo_rot_set_point = MECA_FISH_ROT_CHECK;
            //servo_lr_set_point = MECA_FISH_LR_CENTER;
            //servo_ud_set_point = MECA_FISH_UD_CHECK;
            //meca_fish_update();

            vTaskDelay(750 / portTICK_PERIOD_MS);

            ir_value = platform_adc_get_mV(PLATFORM_ADC_CH0);

            servo_lr_set_point = MECA_FISH_LR_DISABLE;
            servo_ud_set_point = MECA_FISH_UD_DISABLE;
            meca_fish_update();
          }
          break;


        default:
          break;
      }

      xEventGroupSetBits(busy, 0xFF);
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

void meca_fish_close(int wait)
{
  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == MECA_FISH_CLOSE)
  {
    wait = 0;
  }

  next_state = MECA_FISH_CLOSE;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_prepare(int wait)
{
  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == MECA_FISH_PREPARE)
  {
    wait = 0;
  }

  next_state = MECA_FISH_PREPARE;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_walk(int wait)
{
  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == MECA_FISH_WALK)
  {
    wait = 0;
  }

  next_state = MECA_FISH_WALK;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_sweep_left(int wait, int high)
{
  meca_fish_state_t st = MECA_FISH_SWEEP_LEFT;

  if(high) 
  {
    st = MECA_FISH_SWEEP_HIGH_LEFT;
  }

  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == st)
  {
    wait = 0;
  }

  next_state = st;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

int meca_fish_is_catch(void)
{
  xEventGroupClearBits(busy, 0xFF);

  next_state = MECA_FISH_CHECK;

  xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 

#ifdef AUSBEE_SIM
  return ((rand() % 3) == 0);
#else
  return ir_value > 300;
#endif
}

void meca_fish_sweep_right(int wait, int high)
{
  meca_fish_state_t st = MECA_FISH_SWEEP_RIGHT;

  if(high) 
  {
    st = MECA_FISH_SWEEP_HIGH_RIGHT;
  }

  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == st)
  {
    wait = 0;
  }

  next_state = st;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_swim_left(int wait)
{
  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == MECA_FISH_SWIM_LEFT)
  {
    wait = 0;
  }

  next_state = MECA_FISH_SWIM_LEFT;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_swim_right(int wait)
{
  if(wait)
  {
    xEventGroupClearBits(busy, 0xFF);
  }

  if(current_state == MECA_FISH_SWIM_RIGHT)
  {
    wait = 0;
  }

  next_state = MECA_FISH_SWIM_RIGHT;

  if(wait)
  {
    xEventGroupWaitBits(busy, 0xFF, pdFALSE, pdFALSE, portMAX_DELAY); 
  }
}

void meca_fish_init(void)
{
  disabled = 0;
  busy = xEventGroupCreate();
  next_state = MECA_FISH_MANUAL;
  current_state = MECA_FISH_MANUAL;

  meca_fish_close(0);

  xTaskCreate(meca_fish_task, "fish", 200, NULL, 2, NULL);
}

void meca_fish_disable(void)
{
  disabled = 1;
  servo_rot_set_point = MECA_FISH_ROT_DISABLE;
  servo_lr_set_point = MECA_FISH_LR_DISABLE;
  servo_ud_set_point = MECA_FISH_UD_DISABLE;
  meca_fish_update();
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
        meca_fish_close(0);
      }
      if(strcmp(buf, "prepare") == 0)
      {
        meca_fish_prepare(0);
      }
      if(strcmp(buf, "sweep_left") == 0)
      {
        meca_fish_sweep_left(0, 0);
      }
      if(strcmp(buf, "sweep_right") == 0)
      {
        meca_fish_sweep_right(0, 0);
      }
      if(strcmp(buf, "sweep_high_left") == 0)
      {
        meca_fish_sweep_left(0, 1);
      }
      if(strcmp(buf, "sweep_high_right") == 0)
      {
        meca_fish_sweep_right(0, 1);
      }
      if(strcmp(buf, "swim_left") == 0)
      {
        meca_fish_swim_left(0);
      }
      if(strcmp(buf, "swim_right") == 0)
      {
        meca_fish_swim_right(0);
      }
      if(strcmp(buf, "check") == 0)
      {
        int a = meca_fish_is_catch();
        cocobot_console_send_asynchronous("debug", "%d %d", a, ir_value);
      }
    }

    switch(next_state)
    {
      case MECA_FISH_CLOSE:
        cocobot_console_send_answer("close");
        break;

      case MECA_FISH_WALK:
        cocobot_console_send_answer("walk");
        break;

      case MECA_FISH_PREPARE:
        cocobot_console_send_answer("prepare");
        break;

      case MECA_FISH_SWEEP_LEFT:
        cocobot_console_send_answer("sweep_left");
        break;

      case MECA_FISH_CHECK:
        cocobot_console_send_answer("check");
        break;

      case MECA_FISH_SWEEP_RIGHT:
        cocobot_console_send_answer("sweep_right");
        break;

      case MECA_FISH_SWEEP_HIGH_RIGHT:
        cocobot_console_send_answer("sweep_high_right");
        break;

      case MECA_FISH_SWIM_LEFT:
        cocobot_console_send_answer("swim_left");
        break;

      case MECA_FISH_SWEEP_HIGH_LEFT:
        cocobot_console_send_answer("sweep_high_left");
        break;

      case MECA_FISH_SWIM_RIGHT:
        cocobot_console_send_answer("swim_right");
        break;

      case MECA_FISH_MANUAL:
        cocobot_console_send_answer("manual");
        break;

    }

    return 1;
  }

  if(strcmp(command,"meca_fish_ud_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      next_state = MECA_FISH_MANUAL;
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
      next_state = MECA_FISH_MANUAL;
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

  if(strcmp(command,"meca_fish_ir") == 0)
  {
    cocobot_console_send_answer("%ld", platform_adc_get_mV(PLATFORM_ADC_CH0));
    return 1;
  }

  return 0;
}
