#include <platform.h>
#include <cocobot.h>
#include <malloc_wrapper.h>
#include "motor_control.h"
#include <stm32l4xx.h>

#include "uavcan/cocobot/BrushlessConfig.h"
#include "uavcan/cocobot/SetMotorSpeed.h"

uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;

	//accept configuration
	if ((transfer_type == CanardTransferTypeResponse) &&
			(data_type_id == UAVCAN_COCOBOT_BRUSHLESSCONFIG_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_BRUSHLESSCONFIG_SIGNATURE;
		return true;
	}

	//accept speed setpoint
	if ((transfer_type == CanardTransferTypeRequest) &&
			(data_type_id == UAVCAN_COCOBOT_SETMOTORSPEED_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_SETMOTORSPEED_SIGNATURE;
		return true;
	}

	return false;
}

uint8_t com_on_transfer_received(CanardRxTransfer* transfer)
{
	IF_RESPONSE_RECEIVED(UAVCAN_COCOBOT_BRUSHLESSCONFIG, uavcan_cocobot_BrushlessConfigResponse,
			motor_control_set_config(data.imax, data.max_speed_rpm);
			);

	IF_REQUEST_RECEIVED(UAVCAN_COCOBOT_SETMOTORSPEED, uavcan_cocobot_SetMotorSpeedRequest,
			motor_control_set_setpoint(data.enable, data.rpm);
			);

	return 0;
}



int main(void) {

	//initialisations of mcual and libcocobot
	platform_init();
	__enable_irq();
	cocobot_com_init();
	cocobot_com_set_mode(UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL);
	motor_control_init();
	motor_control_set_config(0.0025,400);
	motor_control_set_setpoint(0, 0);


	//main loop
	for(;;)  {
		//process communication requests
		uint64_t timestamp_us = cocobot_com_process_event();

		//run motor control algorithm
		motor_control_process_event(timestamp_us);
	}

	return 0;
}
