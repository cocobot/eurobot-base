#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "cocobot_arm_action.h"
#include "pcm9685.h"
#include "servo.h"
#include "pump.h"
#include "uavcan/cocobot/Pump.h"
#include "uavcan/cocobot/ServoCmd.h"
#include "uavcan/cocobot/MecaAction.h"

static volatile uint8_t _meca_busy = 0;
static volatile uint8_t _req = 0;
static volatile uint8_t _arm = 0;
static volatile uint8_t _arg = 0;

static void thread(void * arg)
{
  (void)arg;
  cocobot_arm_action_init();
  pump_init();
  cocobot_arm_action_init();

  while(1)
  {
    if(_meca_busy)
    {
      switch(_req)
      {
        case UAVCAN_COCOBOT_MECAACTION_REQUEST_INIT:
          cocobot_arm_action_init();
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_FLOOR:
          cocobot_arm_action_prendre_palais_sol(_arm, 0, 0);
          pump_set_state(_arm, 2);
          while(pump_get_state(_arm) != 1)
          {
            vTaskDelay(100/portTICK_PERIOD_MS);
          }
          pump_set_state(_arm, 2);
          cocobot_arm_action_prise_sol_second_try(_arm, 0, 0);
          while(pump_get_state(_arm) != 1)
          {
            vTaskDelay(100/portTICK_PERIOD_MS);
          }

          cocobot_arm_action_repos_normal(_arm);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_GOLDENIUM:
          cocobot_arm_action_prise_goldenium(_arm, 0);
          pump_set_state(_arm, 2);
          while(pump_get_state(_arm) != 1)
          {
            vTaskDelay(100/portTICK_PERIOD_MS);
          }
          cocobot_arm_action_repos_normal(_arm);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_ACCELL:
          if(_arg == 0)
          {
              cocobot_arm_action_prise_bluenium(_arm, 0);
              pump_set_state(_arm, 2);
          }
          else if(_arg == 2)
          {
              cocobot_arm_action_open_accelerateur(_arm, 0);
              vTaskDelay(100/portTICK_PERIOD_MS);
          }
          else
          {
              pump_set_state(_arm, 2);
              while(pump_get_state(_arm) != 1)
              {
                  vTaskDelay(100/portTICK_PERIOD_MS);
              }
              cocobot_arm_action_repos_normal(_arm);
          }
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DROP_ACCELL:
          cocobot_arm_action_depose_accelerateur_particules(_arm, 0, 0);
#if 0
          pump_set_state(_arm, 0);
          vTaskDelay(1000/portTICK_PERIOD_MS);
          cocobot_arm_action_repos_vide(_arm);
#endif
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_REST_EMPTY:
          cocobot_arm_action_repos_vide(_arm);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DROP_FLOOR:
          cocobot_arm_action_depose_case(_arm, 0);
          pump_set_state(_arm, 0);
          vTaskDelay(2000/portTICK_PERIOD_MS);
          cocobot_arm_action_repos_vide(_arm);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_REST_NORMAL:
          cocobot_arm_action_repos_normal(_arm);
          break;
      }

      _meca_busy = 0;
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  } 
}

uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;

	//accept servo setpoint cmd
	if ((transfer_type == CanardTransferTypeRequest) &&
			(data_type_id == UAVCAN_COCOBOT_SERVOCMD_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_SERVOCMD_SIGNATURE;
		return true;
	}

  if ((transfer_type == CanardTransferTypeRequest) &&
			(data_type_id == UAVCAN_COCOBOT_MECAACTION_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_MECAACTION_SIGNATURE;
		return true;
	}

  if ((transfer_type == CanardTransferTypeRequest) &&
			(data_type_id == UAVCAN_COCOBOT_PUMP_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_PUMP_SIGNATURE;
		return true;
	}

	return false;
}

void com_async(uint64_t timestamp_us)
{
}

uint8_t com_on_transfer_received(CanardRxTransfer* transfer)
{
	IF_REQUEST_RECEIVED(UAVCAN_COCOBOT_SERVOCMD, uavcan_cocobot_ServoCmdRequest,
      if(data.mode)
      {
        servo_set_angle(data.servo_id, data.value);
      }
      else
      {
        servo_set_pwm(data.servo_id, data.value);
      }
);

	IF_REQUEST_RECEIVED(UAVCAN_COCOBOT_PUMP, uavcan_cocobot_PumpRequest,
      pump_set_state(data.pump_id, data.action ? 2 : 0);
);

	IF_REQUEST_RECEIVED(UAVCAN_COCOBOT_MECAACTION, uavcan_cocobot_MecaActionRequest,
      if(data.req != UAVCAN_COCOBOT_MECAACTION_REQUEST_STATUS)
      {
        _arm = data.arm;
        _req = data.req;
        _arg = data.a;
        _meca_busy = 1;
      }
 
      uavcan_cocobot_MecaActionResponse action;

      action.busy = _meca_busy;

      void * buf = pvPortMalloc(UAVCAN_COCOBOT_MECAACTION_RESPONSE_MAX_SIZE); 
      if(buf != NULL) 
      {
        static uint8_t transfer_id;

        const int size = uavcan_cocobot_MecaActionResponse_encode(&action, buf);
        cocobot_com_request_or_respond(transfer->source_node_id,
                                       UAVCAN_COCOBOT_MECAACTION_SIGNATURE,
                                       UAVCAN_COCOBOT_MECAACTION_ID,
                                       &transfer_id,
                                       CANARD_TRANSFER_PRIORITY_LOW,
                                       CanardResponse,
                                       buf,
                                       (uint16_t)size);
        vPortFree(buf);
      }
  );

	return 0;
}


int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);

  xTaskCreate(thread, "thread", 1024, NULL, 3, NULL);
  vTaskStartScheduler();

  return 0;
}
