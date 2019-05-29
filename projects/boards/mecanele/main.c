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

static void thread(void * arg)
{
  (void)arg;
  pump_init();

  while(1)
  {
    //int i;
    //for(i =0; i < 10; i += 1)
    //{
    //  platform_gpio_set(1 << i);
    //  vTaskDelay(100/portTICK_PERIOD_MS);
    //  platform_gpio_clear(1 << i);
    //  vTaskDelay(100/portTICK_PERIOD_MS);
    //}

    vTaskDelay(100/portTICK_PERIOD_MS);
  } 
}

uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;
		return false;

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
      switch(data.req)
      {
        case UAVCAN_COCOBOT_MECAACTION_REQUEST_STATUS:
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_INIT:
          cocobot_arm_action_init();
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_DISTRIB:
          cocobot_arm_action_prise_distributeur(data.arm, data.a);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_ACCELL:
          cocobot_arm_action_prise_bluenium(data.arm, data.a);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_GOLDENIUM:
          cocobot_arm_action_prise_goldenium(data.arm, data.a);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_TAKE_FLOOR:
          cocobot_arm_action_prendre_palais_sol(data.arm, data.x, data.y);

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_REST_EMPTY:
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_REST_NORMAL:
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_REST_GLODNENIUM:
          cocobot_arm_action_repos_goldenium(data.arm);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DROP_FLOOR:
          cocobot_arm_action_depose_case(data.arm, data.a);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DROP_BALANCE:
          cocobot_arm_action_depose_balance(data.arm, data.a);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DROP_ACCELL:
          cocobot_arm_action_depose_accelerateur_particules(data.arm, data.a, data.d);
          break;

        case UAVCAN_COCOBOT_MECAACTION_REQUEST_DIRECT_ARM:
          cocobot_arm_action_move_arm(data.arm, ((float)data.x) / 1000.0f, ((float)data.y) / 1000.0f, ((float)data.z) / 1000.0f, data.a);
          break;
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
