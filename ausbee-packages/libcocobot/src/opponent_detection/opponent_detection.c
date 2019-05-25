#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_OPPONENT_DETECTION

#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>
#include <platform.h>
#include <math.h>
#include "uavcan/cocobot/Collision.h"

#define COCOBOT_OPPONENT_MIN_CORRELATION  150

typedef struct
{
  int alert_activated;
  int alert;
  int force_on;
} cocobot_opponent_detection_usir_t;


static cocobot_opponent_detection_usir_t _alerts[4];
static uavcan_cocobot_CollisionRequest col;
static uint8_t buf[UAVCAN_COCOBOT_COLLISION_REQUEST_MAX_SIZE];
static uint8_t transfer_id;


void cocobot_opponent_detection_task(void * arg)
{
  //arg is always NULL. Prevent "variable unused" warning
  (void)arg;

  while(1)
  {
    col.x = cocobot_position_get_x();
    col.y = cocobot_position_get_y();
    col.a = cocobot_position_get_angle();
    const uint16_t size = uavcan_cocobot_CollisionRequest_encode(&col, &buf[0]);
    cocobot_com_request_or_respond(COCOBOT_LIDAR_NODE_ID,
                        UAVCAN_COCOBOT_COLLISION_SIGNATURE,
                        UAVCAN_COCOBOT_COLLISION_ID,
                        &transfer_id,
                        CANARD_TRANSFER_PRIORITY_HIGH,
                        CanardRequest,
                        &buf[0],
                        size);

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

int cocobot_opponent_detection_is_in_alert(void)
{
  int i;

  for(i = 0; i < 4; i += 1)
  {
    if((_alerts[i].alert == 1) && (_alerts[i].alert_activated))
    {
      return 1;
    }
  }

  return 0;
}

void cocobot_opponent_detection_set_enable(int id, int status)
{
  _alerts[id].alert_activated = status;
}

void cocobot_opponent_detection_init(unsigned int task_priority)
{
  (void)task_priority;

  int i;
  for(i = 0; i < 4; i += 1)
  {
    _alerts[i].alert_activated = COCOBOT_OPPONENT_DETECTION_DEACTIVATED;
    _alerts[i].alert = 0;
    _alerts[i].force_on = 0;
  }

  //start task
  xTaskCreate(cocobot_opponent_detection_task, "opponent", 512, NULL, task_priority, NULL);
}

#endif
