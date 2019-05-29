#include <mcual.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cocobot.h>
#include "uavcan/cocobot/Config.h"

uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
		uint16_t data_type_id,
		CanardTransferType transfer_type,
		uint8_t source_node_id)
{
	(void)source_node_id;

	if ((transfer_type == CanardTransferTypeRequest) &&
			(data_type_id == UAVCAN_COCOBOT_CONFIG_ID)
		 )
	{
		*out_data_type_signature = UAVCAN_COCOBOT_CONFIG_SIGNATURE;
		return true;
	}

	return false;
}

uint8_t com_on_transfer_received(CanardRxTransfer* transfer)
{
	IF_REQUEST_RECEIVED(UAVCAN_COCOBOT_CONFIG, uavcan_cocobot_ConfigRequest,
      uint8_t raw_config = platform_gpio_get(0xFF);
      uavcan_cocobot_ConfigResponse config;

      config.config = raw_config;

      void * buf = pvPortMalloc(UAVCAN_COCOBOT_CONFIG_RESPONSE_MAX_SIZE); 
      if(buf != NULL) 
      {
        static uint8_t transfer_id;

        const int size = uavcan_cocobot_ConfigResponse_encode(&config, buf);
        cocobot_com_broadcast(UAVCAN_COCOBOT_CONFIG_SIGNATURE,
                              UAVCAN_COCOBOT_CONFIG_ID,
                              &transfer_id,
                              CANARD_TRANSFER_PRIORITY_LOW,
                              buf,
                              (uint16_t)size);
        vPortFree(buf);

        if(config.config & (1 << 0))
        {
          platform_led_set(PLATFORM_LED_RED_1);
        }
        else
        {
          platform_led_set(PLATFORM_LED_RED_1);
          platform_led_set(PLATFORM_LED_GREEN_1);
        }
      }
  );

	return 0;
}
 
int main(void) 
{
  platform_init();

  cocobot_com_init();
  cocobot_com_run();

  vTaskStartScheduler();

  return 0;
}
