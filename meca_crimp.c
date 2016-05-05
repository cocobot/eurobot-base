#include <string.h>
#include <cocobot.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <stm32f4xx.h>
#include "meca_crimp.h"

#include <cocobot/asserv_pid.h>

//max 1050 (ferme)
//min 688 (ouvert)

#define MECA_CRIMP_MAX_OPENNING 100

#define MECA_CRIMP_LEFT_SERVO_ID         PLATFORM_SERVO_15_ID
#define MECA_CRIMP_RIGHT_SERVO_ID        PLATFORM_SERVO_3_ID
#define MECA_CRIMP_VERTICAL_SERVO_ID     PLATFORM_SERVO_14_ID

#define MECA_CRIMP_LEFT_SERVO_OPEN 200
#define MECA_CRIMP_LEFT_SERVO_CLOSE 550
#define MECA_CRIMP_LEFT_SERVO_DISABLE 0

#define MECA_CRIMP_RIGHT_SERVO_OPEN 340
#define MECA_CRIMP_RIGHT_SERVO_CLOSE 500
#define MECA_CRIMP_RIGHT_SERVO_DISABLE 0

#define MECA_CRIMP_VERTICAL_SERVO_GROUND 0
#define MECA_CRIMP_VERTICAL_SERVO_FENCE 0
#define MECA_CRIMP_VERTICAL_SERVO_F1 0
#define MECA_CRIMP_VERTICAL_SERVO_DISABLE 0

static unsigned int servo_left_set_point;
static unsigned int servo_right_set_point;
static unsigned int servo_vertical_set_point;

static volatile int crimp_disable;
static volatile int crimp_current;
static volatile int crimp_target;
static volatile int32_t last_output;

static cocobot_asserv_pid_t pid;

static void meca_crimp_update_servo(void)
{
  platform_servo_set_value(MECA_CRIMP_LEFT_SERVO_ID, servo_left_set_point);
  platform_servo_set_value(MECA_CRIMP_RIGHT_SERVO_ID, servo_right_set_point);
  platform_servo_set_value(MECA_CRIMP_VERTICAL_SERVO_ID, servo_vertical_set_point);
}

static void meca_crimp_task(void * arg)
{
  (void)arg;
  while(1)
  {
    crimp_current = platform_adc_get_mV(PLATFORM_ADC_CH0);

    if(crimp_disable)
    {
      platform_gpio_clear(PLATFORM_GPIO1);
      cocobot_asserv_pid_reset(&pid);
    }
    else
    {
      platform_gpio_set(PLATFORM_GPIO1);
      cocobot_asserv_pid_set_input(&pid, crimp_target);
      cocobot_asserv_pid_set_feedback(&pid, crimp_current);

      cocobot_asserv_pid_compute(&pid);

      int32_t output = cocobot_asserv_pid_get_output(&pid);
      last_output = output;
      cocobot_asserv_pid_compute(&pid);

      if(output > 0)
      {
        if(output > 0xFFFF)
        {
          output = 0xFFFF;
        }

        platform_gpio_clear(PLATFORM_GPIO6);
        mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL4, output);
      }
      else
      {
        output = -output;
        if(output > 0xFFFF)
        {
          output = 0xFFFF;
        }
        platform_gpio_set(PLATFORM_GPIO6);
        mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL4, 0xFFFF - output);
      }
    }
    
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void meca_crimp_init(void)
{


  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_DISABLE;
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_DISABLE;
  servo_vertical_set_point = MECA_CRIMP_VERTICAL_SERVO_DISABLE;

  platform_gpio_set_direction(PLATFORM_GPIO1, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO6, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO7, MCUAL_GPIO_OUTPUT);

  platform_gpio_clear(PLATFORM_GPIO1);
  platform_gpio_clear(PLATFORM_GPIO6);
  platform_gpio_clear(PLATFORM_GPIO7);

  mcual_timer_init(MCUAL_TIMER1, 20000);
  mcual_timer_set_duty_cycle(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL4, 0x0000);
  mcual_timer_enable_channel(MCUAL_TIMER1, MCUAL_TIMER_CHANNEL4);
  mcual_gpio_set_function(MCUAL_GPIOE, MCUAL_GPIO_PIN14, 1);

  platform_gpio_clear(PLATFORM_GPIO1);

  cocobot_asserv_pid_init(&pid);
  cocobot_asserv_pid_set_kp(&pid, 750.0);
  cocobot_asserv_pid_set_kd(&pid, 1000.0);


  meca_crimp_set_target(750);
  crimp_disable = 0;

  xTaskCreate(meca_crimp_task, "crimp", 200, NULL, 2, NULL);

  meca_crimp_close();
}

void meca_crimp_open(void)
{
  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_OPEN;
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_OPEN;

  meca_crimp_update_servo();
}

void meca_crimp_close(void)
{
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_CLOSE;
  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_CLOSE;
  meca_crimp_update_servo();
}

void meca_crimp_disable(void)
{
  //crimp_target = 891;//MECA_CRIMP_MAX_OPENNING;
  crimp_disable = 1;

  servo_left_set_point = MECA_CRIMP_LEFT_SERVO_DISABLE;
  servo_right_set_point = MECA_CRIMP_RIGHT_SERVO_DISABLE;
  servo_vertical_set_point = MECA_CRIMP_VERTICAL_SERVO_DISABLE;
  meca_crimp_update_servo();
}

int meca_crimp_console_handler(const char * command)
{
  if(strcmp(command,"meca_crimp_left_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_left_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_left_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_right_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_right_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_right_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_vertical_servo") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      servo_vertical_set_point = set;
      meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", servo_vertical_set_point);
    return 1;
  }

  if(strcmp(command,"meca_crimp_target") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      crimp_target = set;
      crimp_disable = 0;
     // meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", crimp_target);
    return 1;
  }

  if(strcmp(command,"meca_crimp_current") == 0)
  {
    int set;
    if(cocobot_console_get_iargument(0, &set))
    {
      crimp_current = set;
     // meca_crimp_update_servo();
    }
    cocobot_console_send_answer("%u", crimp_current);
    return 1;
  }

  if(strcmp(command,"meca_crimp_kp") == 0)
  {
    float set;
    if(cocobot_console_get_fargument(0, &set))
    {
      cocobot_asserv_pid_set_kp(&pid, set);
    }
    cocobot_console_send_answer("%.3f", (double)cocobot_asserv_pid_get_kp(&pid));
    return 1;
  }

  if(strcmp(command,"meca_crimp_kd") == 0)
  {
    float set;
    if(cocobot_console_get_fargument(0, &set))
    {
      cocobot_asserv_pid_set_kd(&pid, set);
    }
    cocobot_console_send_answer("%.3f", (double)cocobot_asserv_pid_get_kd(&pid));
    return 1;
  }

  if(strcmp(command,"meca_crimp_open") == 0)
  {
    meca_crimp_open();
    cocobot_console_send_answer("open");
    return 1;
  }

  if(strcmp(command,"meca_crimp_close") == 0)
  {
    meca_crimp_close();
    cocobot_console_send_answer("close");
    return 1;
  }

  if(strcmp(command,"debug") == 0)
  {
    cocobot_console_send_answer("RTSR 0x%04lX",  EXTI->RTSR);
    cocobot_console_send_answer("FTSR 0x%04lX",  EXTI->FTSR);
    cocobot_console_send_answer("IMR  0x%04lX",  EXTI->IMR);
    cocobot_console_send_answer("EXTICR0  0x%04lX",  SYSCFG->EXTICR[0]);
    cocobot_console_send_answer("EXTICR1  0x%04lX",  SYSCFG->EXTICR[1]);
    cocobot_console_send_answer("EXTICR2  0x%04lX",  SYSCFG->EXTICR[2]);
    cocobot_console_send_answer("EXTICR3  0x%04lX",  SYSCFG->EXTICR[3]);
    return 1;
  }

  return 0;
}

void meca_crimp_set_target(int target)
{
  crimp_target = target;
}

void meca_crimp_set_vertical(int target)
{
  servo_vertical_set_point = target;
  meca_crimp_update_servo();
}
