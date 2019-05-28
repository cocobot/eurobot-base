#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_ASSERV

#include <cocobot.h>
#include <platform.h>
#include <math.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "generated/autoconf.h"
#include "uavcan/cocobot/Position.h"
#include "uavcan/cocobot/SetMotorSpeed.h"

#include <cocobot/encoders.h>

#include "asserv_data.h"

//useful macros
#define TICK2RAD(tick)  ((((float)tick) * M_PI) / ((float)CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG))
#define TICK2DEG(tick)  ((((float)tick) * 180.0) / ((float)CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG))
#define TICK2MM(tick)  ((((float)tick) * 1000.0) / ((float)CONFIG_LIBCOCOBOT_POSITION_TICK_PER_METER))
#define MM2TICK(mm)  (((float)mm) * ((float)CONFIG_LIBCOCOBOT_POSITION_TICK_PER_METER) / 1000.0)
#define DEG2TICK(deg)  ((((float)deg) / 180.0) * ((float)CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG))

//mutex for internal value access
static SemaphoreHandle_t mutex;

//internal values (in tick)
static float robot_x=0, robot_y=0;
static int32_t robot_distance=0,     robot_angle=0, robot_angle_offset=0;
static int32_t robot_linear_speed=0, robot_angular_velocity=0;
int32_t motor_position[2] = {0, 0}; // {right, left}
static float last_left_sp = 0;
static float last_right_sp = 0;
static float left_motor_alpha = (((float)CONFIG_LIBCOCOBOT_LEFT_MOTOR_ALPHA) / 1000.0f);
static float right_motor_alpha = (((float)CONFIG_LIBCOCOBOT_RIGHT_MOTOR_ALPHA) / 1000.0f);
static uint64_t _next_10hz_service_at;
static uint8_t _request_send_motor = 0;

#ifdef COCOBOT_CAN_MOTOR
static uavcan_cocobot_SetMotorSpeedRequest st_left;
static uavcan_cocobot_SetMotorSpeedRequest st_right;
static uint8_t buf_left[UAVCAN_COCOBOT_SETMOTORSPEED_REQUEST_MAX_SIZE];
static uint8_t buf_right[UAVCAN_COCOBOT_SETMOTORSPEED_REQUEST_MAX_SIZE];
static uint8_t transfer_id_left;
static uint8_t transfer_id_right;
#endif

static void cocobot_position_compute(void)
{
  //update encoder values
  cocobot_encoders_get_motor_position(motor_position);

  //compute new curvilinear distance
  int32_t new_distance = motor_position[0] + motor_position[1];
  int32_t delta_distance = new_distance - robot_distance;

  //compute new angle value
  int32_t new_angle = motor_position[0] - motor_position[1] + robot_angle_offset;
  int32_t delta_angle = new_angle - robot_angle;

  //compute X/Y coordonate
  float mid_angle = TICK2RAD(robot_angle + delta_angle / 2);
  float dx = delta_distance * cos(mid_angle);
  float dy = delta_distance * sin(mid_angle);
  robot_x += dx;
  robot_y += dy;

  robot_angle = new_angle;
  robot_distance = new_distance;
  robot_linear_speed = delta_distance;
  robot_angular_velocity = delta_angle;
}

#include <stdio.h>
static void cocobot_position_task(void * arg)
{
  //arg is always NULL. Prevent "variable unused" warning
  (void)arg;

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while(1)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);

    cocobot_position_compute();

    xSemaphoreGive(mutex);

    //run the asserv
    cocobot_asserv_compute();

    //wait 10ms
    vTaskDelayUntil( &xLastWakeTime, 10 / portTICK_PERIOD_MS);
  }
}


void cocobot_position_init(unsigned int task_priority)
{
  //create mutex
  mutex = xSemaphoreCreateMutex();

  _next_10hz_service_at = 0;
  _request_send_motor = 0;

  //Be sure that position manager have valid values before continuing the initialization process.
  //Need to run twice in order to intialize speed values
  cocobot_position_compute();
  cocobot_position_compute();

#ifdef AUSBEE_SIM
  mcual_arch_request("POS", 0, "INIT:%lu:%lu", CONFIG_LIBCOCOBOT_POSITION_TICK_PER_METER, CONFIG_LIBCOCOBOT_POSITION_TICK_PER_180DEG);
#endif

  //Start task
  xTaskCreate(cocobot_position_task, "position", 200, NULL, task_priority, NULL);
}

float cocobot_position_get_x(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  float x = TICK2MM(robot_x);
  xSemaphoreGive(mutex);

  return x;
}

float cocobot_position_get_y(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  float y = TICK2MM(robot_y);
  xSemaphoreGive(mutex);

  return y;
}

float cocobot_position_get_distance(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  float d = TICK2MM(robot_distance);
  xSemaphoreGive(mutex);

  return d;
}

float cocobot_position_get_angle(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  double a = TICK2DEG(robot_angle);
  xSemaphoreGive(mutex);

  return a;
}

float cocobot_position_get_speed_distance(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  float d = TICK2MM(robot_linear_speed);
  xSemaphoreGive(mutex);

  return d;
}

float cocobot_position_get_speed_angle(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  double a = TICK2DEG(robot_angular_velocity);
  xSemaphoreGive(mutex);

  return a;
}

int32_t cocobot_position_get_left_encoder(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  int32_t v = motor_position[0];
  xSemaphoreGive(mutex);

  return v;
}

int32_t cocobot_position_get_right_encoder(void)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  int32_t v = motor_position[1];
  xSemaphoreGive(mutex);

  return v;
}

void cocobot_position_set_motor_command(float left_motor_speed, float right_motor_speed)
{
#ifdef COCOBOT_CAN_MOTOR
  if((cocobot_asserv_get_state() == COCOBOT_ASSERV_ENABLE) && !cocobot_opponent_detection_is_in_alert())
  {
    st_left.enable = 1;
    st_right.enable = 1;
  }
  else
  {
    st_left.enable = 0;
    st_right.enable = 0;
  }

#ifdef COCOBOT_INVERT_LEFT_MOTOR
  left_motor_speed = -left_motor_speed;
#endif
#ifdef COCOBOT_INVERT_RIGHT_MOTOR
  right_motor_speed = -right_motor_speed;
#endif

  xSemaphoreTake(mutex, portMAX_DELAY);
  st_left.rpm = left_motor_speed; 
  st_right.rpm = right_motor_speed; 
  _request_send_motor = 1;
  xSemaphoreGive(mutex);
#else
  if(left_motor_speed > 0xffff)
  {
    left_motor_speed = 0xffff;
  }
  if(left_motor_speed < -0xffff)
  {
    left_motor_speed = -0xffff;
  }
  if(right_motor_speed > 0xffff)
  {
    right_motor_speed = 0xffff;
  }
  if(right_motor_speed < -0xffff)
  {
    right_motor_speed = -0xffff;
  }

#ifdef AUSBEE_SIM
#ifdef VREP_ENABLE
    cocobot_vrep_set_motor_command(left_motor_speed, right_motor_speed);
#else
    motor_position[0] += right_motor_speed / 100.0f;
    motor_position[1] += left_motor_speed / 100.0f;
#endif
#else

#ifdef COCOBOT_INVERT_LEFT_MOTOR
  left_motor_speed = -left_motor_speed;
#endif
#ifdef COCOBOT_INVERT_RIGHT_MOTOR
  right_motor_speed = -right_motor_speed;
#endif


  if(left_motor_speed >= 0)
  {
#ifdef COCOBOT_LMD18200T
    platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_LEFT);
    platform_motor_set_left_duty_cycle(left_motor_speed);
#endif
#ifdef COCOBOT_L298
    left_motor_speed = 0xFFFF - left_motor_speed;
    platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_LEFT);
    platform_motor_set_left_duty_cycle(left_motor_speed);
#endif
  }
  else
  {
    left_motor_speed = -left_motor_speed;

#ifdef COCOBOT_LMD18200T
    platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_LEFT);
    platform_motor_set_left_duty_cycle(left_motor_speed);
#endif
#ifdef COCOBOT_L298
    platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_LEFT);
    platform_motor_set_left_duty_cycle(left_motor_speed);
#endif
  }

  if(right_motor_speed >= 0)
  {
#ifdef COCOBOT_LMD18200T
    platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_RIGHT);
    platform_motor_set_right_duty_cycle(right_motor_speed);
#endif

#ifdef COCOBOT_L298
    right_motor_speed = 0xFFFF - right_motor_speed;
    platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_RIGHT);
    platform_motor_set_right_duty_cycle(right_motor_speed);
#endif
  }
  else
  {
    right_motor_speed = -right_motor_speed;

#ifdef COCOBOT_LMD18200T
    platform_gpio_set(PLATFORM_GPIO_MOTOR_DIR_RIGHT);
    platform_motor_set_right_duty_cycle(right_motor_speed);
#endif

#ifdef COCOBOT_L298
    platform_gpio_clear(PLATFORM_GPIO_MOTOR_DIR_RIGHT);
    platform_motor_set_right_duty_cycle(right_motor_speed);
#endif
  }
#endif //AUSBEE_SIM
#endif
}

void cocobot_position_set_speed_distance_angle(float linear_speed, float angular_velocity)
{
#ifdef AUSBEE_SIM
  mcual_arch_request("POS", 0, "SPEED:%f:%f", linear_speed * 100, angular_velocity * 100);
#endif
  float c1 = linear_speed + angular_velocity;
  float c2 = linear_speed - angular_velocity;

  float k1 = 1;
  float k2 = 1;

  if(fabsf(c1) > 0xFFFF)
  {
    k1 = ((float)0xFFFF) / fabsf(c1);
  }
  if(fabsf(c2) > 0xFFFF)
  {
    k2 = ((float)0xFFFF) / fabsf(c2);
  }

  if(k2 < k1)
  {
    k1 = k2;
  }

  float left_sp = k1 * (linear_speed-angular_velocity);
  float right_sp = k1 * (linear_speed+angular_velocity);

  left_sp = last_left_sp + left_motor_alpha * (left_sp - last_left_sp);
  right_sp = last_right_sp + right_motor_alpha * (right_sp - last_right_sp);

  cocobot_position_set_motor_command(left_sp, right_sp);
}

void cocobot_position_set_x(float x)
{
  cocobot_asserv_state_t saved_state = cocobot_asserv_get_state();

  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

  xSemaphoreTake(mutex, portMAX_DELAY);
  robot_x = MM2TICK(x);
  xSemaphoreGive(mutex);
#ifdef AUSBEE_SIM
  mcual_arch_request("POS", 0, "X:%f", x);
#endif

  cocobot_asserv_set_state(saved_state);
}

void cocobot_position_set_y(float y)
{
  cocobot_asserv_state_t saved_state = cocobot_asserv_get_state();

  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

  xSemaphoreTake(mutex, portMAX_DELAY);
  robot_y = MM2TICK(y);
  xSemaphoreGive(mutex);
#ifdef AUSBEE_SIM
  mcual_arch_request("POS", 0, "Y:%f", y);
#endif

  cocobot_asserv_set_state(saved_state);
}

void cocobot_position_set_angle(float angle)
{
  cocobot_asserv_state_t saved_state = cocobot_asserv_get_state();

  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

  xSemaphoreTake(mutex, portMAX_DELAY);
  float old_angle = TICK2DEG(robot_angle);
  float diff = angle - old_angle;
  robot_angle_offset += DEG2TICK(diff);
  xSemaphoreGive(mutex);
#ifdef AUSBEE_SIM
  mcual_arch_request("POS", 0, "A:%f", angle);
#endif

  cocobot_asserv_set_state(saved_state);
}


void cocobot_position_com_async(uint64_t timestamp_us)
{
  if(_request_send_motor)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);
    _request_send_motor = 0;

    const uint16_t left_size = uavcan_cocobot_SetMotorSpeedRequest_encode(&st_left, &buf_left[0]);

    cocobot_com_request_or_respond(COCOBOT_LEFT_MOTOR_NODE_ID,
                                   UAVCAN_COCOBOT_SETMOTORSPEED_SIGNATURE,
                                   UAVCAN_COCOBOT_SETMOTORSPEED_ID,
                                   &transfer_id_left,
                                   CANARD_TRANSFER_PRIORITY_HIGH,
                                   CanardRequest,
                                   &buf_left[0],
                                   left_size);

    const uint16_t right_size = uavcan_cocobot_SetMotorSpeedRequest_encode(&st_right, &buf_right[0]);

    cocobot_com_request_or_respond(COCOBOT_RIGHT_MOTOR_NODE_ID,
                                   UAVCAN_COCOBOT_SETMOTORSPEED_SIGNATURE,
                                   UAVCAN_COCOBOT_SETMOTORSPEED_ID,
                                   &transfer_id_right,
                                   CANARD_TRANSFER_PRIORITY_HIGH,
                                   CanardRequest,
                                   &buf_right[0],
                                   right_size);
    xSemaphoreGive(mutex);
  }

  if (timestamp_us >= _next_10hz_service_at)
  {
    _next_10hz_service_at = timestamp_us + 100000;

    uavcan_cocobot_Position pos;

    pos.x = cocobot_position_get_x();
    pos.y = cocobot_position_get_y();
    pos.a = cocobot_position_get_angle();

    void * buf = pvPortMalloc(UAVCAN_COCOBOT_POSITION_MAX_SIZE); 
    if(buf != NULL) 
    {
      static uint8_t transfer_id;

      const int size = uavcan_cocobot_Position_encode(&pos, buf);
      cocobot_com_broadcast(UAVCAN_COCOBOT_POSITION_SIGNATURE,
                            UAVCAN_COCOBOT_POSITION_ID,
                            &transfer_id,
                            CANARD_TRANSFER_PRIORITY_LOW,
                            buf,
                            (uint16_t)size);
      vPortFree(buf);
    }
  }
}

#endif
