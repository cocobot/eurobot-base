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
static uint64_t _next_service_at;


void cocobot_opponent_detection_task(void * arg)
{
  //arg is always NULL. Prevent "variable unused" warning
  (void)arg;

  while(1)
  {
    //col.x = cocobot_position_get_x();
    //col.y = cocobot_position_get_y();
    //col.a = cocobot_position_get_angle();
    //const uint16_t size = uavcan_cocobot_CollisionRequest_encode(&col, &buf[0]);
    //cocobot_com_request_or_respond(COCOBOT_LIDAR_NODE_ID,
    //                    UAVCAN_COCOBOT_COLLISION_SIGNATURE,
    //                    UAVCAN_COCOBOT_COLLISION_ID,
    //                    &transfer_id,
    //                    CANARD_TRANSFER_PRIORITY_HIGH,
    //                    CanardRequest,
    //                    &buf[0],
    //                    size);

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void cocobot_opponent_detection_com_async(uint64_t timestamp_us)
{
  if (timestamp_us >= _next_service_at)
  {
    _next_service_at = timestamp_us + 75000;

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

  }
}

uint8_t cocobot_opponent_detection_should_accept_transfer(uint64_t* out_data_type_signature,
                                              uint16_t data_type_id,
                                              CanardTransferType transfer_type,
                                              uint8_t source_node_id)
{
  if ((transfer_type == CanardTransferTypeResponse) &&
      (data_type_id == UAVCAN_COCOBOT_COLLISION_ID) &&
      (source_node_id == COCOBOT_LIDAR_NODE_ID))
  {
    *out_data_type_signature = UAVCAN_COCOBOT_COLLISION_SIGNATURE;
    return true;
  }

  return false;
}

uint8_t cocobot_opponent_detection_on_transfer_received(CanardRxTransfer* transfer)
{
  if ((transfer->transfer_type == CanardTransferTypeResponse) &&
      (transfer->data_type_id == UAVCAN_COCOBOT_COLLISION_ID))
  {
    uavcan_cocobot_CollisionResponse col; 


    void * dynbuf = NULL;

    //get file bytes
    dynbuf = pvPortMalloc(UAVCAN_COCOBOT_COLLISION_RESPONSE_MAX_SIZE);
    if(dynbuf != NULL)
    {
      uint8_t * pdynbuf = dynbuf;
      if(uavcan_cocobot_CollisionResponse_decode(transfer, transfer->payload_len, &col, &pdynbuf) >= 0)
      {
        _alerts[COCOBOT_OPPONENT_DETECTION_FRONT_LEFT].alert = col.left_front;
        _alerts[COCOBOT_OPPONENT_DETECTION_FRONT_RIGHT].alert = col.right_front;
        _alerts[COCOBOT_OPPONENT_DETECTION_BACK_LEFT].alert = col.left_back;
        _alerts[COCOBOT_OPPONENT_DETECTION_BACK_RIGHT].alert = col.right_back;

        //release rx memory before tx
        cocobot_com_release_rx_transfer_payload(transfer);
      }
    }

    //free memory
    if(dynbuf != NULL)
    {
      vPortFree(dynbuf);
    }

    return 1;
  }

  return 0;
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

  _next_service_at = 1000;
}

#endif
