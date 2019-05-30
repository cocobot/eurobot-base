#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>
#include <semphr.h>
#include "uavcan/cocobot/MecaAction.h"

static volatile uint8_t _meca_busy = 0;
static uint64_t _next_10hz_service_at = 0;
static volatile uint8_t _req = 0;
static volatile uint8_t _req_needed = 0;
static volatile uint8_t _arm = 0;
static SemaphoreHandle_t _mutex;

uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;

  if ((transfer_type == CanardTransferTypeResponse) &&
			(data_type_id == UAVCAN_COCOBOT_MECAACTION_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_MECAACTION_SIGNATURE;
		return true;
	}

	return false;
}

uint8_t com_on_transfer_received(CanardRxTransfer* transfer)
{

	IF_RESPONSE_RECEIVED(UAVCAN_COCOBOT_MECAACTION, uavcan_cocobot_MecaActionResponse,
      _meca_busy = data.busy;
  );

	return 0;
}

void com_async(uint64_t timestamp_us)
{
  if (
      ((timestamp_us >= _next_10hz_service_at) && _meca_busy)
      ||
      (_req_needed)
     )
  {
    _next_10hz_service_at = timestamp_us + 100000;

    uavcan_cocobot_MecaActionRequest action;

    xSemaphoreTake(_mutex, portMAX_DELAY);
    action.req = _req;
    action.arm = _arm;
    action.x = 0;
    action.y = 0;
    action.z = 0;
    action.a = 0;
    action.d = 0;

    _req = 0;
    if(_req_needed)
    {
      _req_needed = 0;
      _meca_busy = 1;
    }
    xSemaphoreGive(_mutex);


    void * buf = pvPortMalloc(UAVCAN_COCOBOT_MECAACTION_REQUEST_MAX_SIZE); 
    if(buf != NULL) 
    {
      static uint8_t transfer_id;

      const int size = uavcan_cocobot_MecaActionRequest_encode(&action, buf);
      cocobot_com_request_or_respond(
                            COCOBOT_MECA_NODE_ID,
                            UAVCAN_COCOBOT_MECAACTION_SIGNATURE,
                            UAVCAN_COCOBOT_MECAACTION_ID,
                            &transfer_id,
                            CANARD_TRANSFER_PRIORITY_LOW,
                            CanardRequest,
                            buf,
                            (uint16_t)size);
      vPortFree(buf);
    }
  }
}

void meca_action(uint8_t arm_id, uint8_t req)
{

  xSemaphoreTake(_mutex, portMAX_DELAY);
  _arm = arm_id;
  _req = req;
  _meca_busy = 1;
  _req_needed = 1;
  xSemaphoreGive(_mutex);

  while(_meca_busy)
  {
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void meca_init(void)
{
  _mutex = xSemaphoreCreateMutex();
}
