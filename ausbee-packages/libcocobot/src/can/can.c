#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_CAN

#include <cocobot.h>
#include <mcual.h>
#include <platform.h>
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <task.h>
#else
# include <malloc_wrapper.h>
#endif
#include <canard.h>
#include <canard_stm32.h>
#include <unistd.h>
#include "dsdl/uavcan/protocol/GetNodeInfo.h"
#include "dsdl/uavcan/protocol/RestartNode.h"
#include "dsdl/uavcan/protocol/NodeStatus.h"
#include <include/generated/git.h>

static CanardInstance _canard;
static uint8_t _canard_memory_pool[CONFIG_LIBCOCOBOT_CAN_MEMORY_POOL_SIZE];
static uint8_t _internal_buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
static uint8_t _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
static uint8_t _mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION;
static uint16_t _last_timer_ticks;
static uint64_t _timestamp_ms;


static void cocobot_can_on_transfer_received(CanardInstance* ins,
                                             CanardRxTransfer* transfer)
{
  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
      (transfer->data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
  {
    canardReleaseRxTransferPayload(ins, transfer);

    uavcan_protocol_GetNodeInfoResponse nir; 
    nir.status.uptime_sec = _timestamp_ms;
    nir.status.health = _health;
    nir.status.mode = _mode;
    nir.status.sub_mode = 0;
    nir.status.vendor_specific_status_code = 0;
    nir.software_version.major = 19; 
    nir.software_version.minor = 0; 
    nir.software_version.optional_field_flags = UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_VCS_COMMIT; 
    nir.software_version.vcs_commit = GIT_COMMIT_SHORT8_ID; 
    nir.software_version.image_crc = 0; 
    nir.hardware_version.major = PLATFORM_MAJOR; 
    nir.hardware_version.minor = PLATFORM_MINOR; 
    mcual_get_unique_id(nir.hardware_version.unique_id);
    nir.hardware_version.unique_id[12] = 0; 
    nir.hardware_version.unique_id[13] = 0; 
    nir.hardware_version.unique_id[14] = 0; 
    nir.hardware_version.unique_id[15] = 0; 
    nir.hardware_version.certificate_of_authenticity.len = 0; 
    nir.hardware_version.certificate_of_authenticity.data = NULL; 
    nir.name.data = (uint8_t *)PROJECT_NAME; 
    nir.name.len = strlen(PROJECT_NAME);

    const uint32_t size = uavcan_protocol_GetNodeInfoResponse_encode(&nir, &_internal_buffer[0]);

    const int16_t resp_res = canardRequestOrRespond(ins,
                                                    transfer->source_node_id,
                                                    UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
                                                    UAVCAN_PROTOCOL_GETNODEINFO_ID,
                                                    &transfer->transfer_id,
                                                    transfer->priority,
                                                    CanardResponse,
                                                    &_internal_buffer[0],
                                                    (uint16_t)size);
    if (resp_res <= 0)
    {
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }

    return;
  }

  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
      (transfer->data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
  {
    uavcan_protocol_RestartNodeResponse rnr; 
    rnr.ok = 0; 

    uavcan_protocol_RestartNodeRequest reqrnr; 
    void * dynbuf = NULL;
    uint8_t * pdynbuf = dynbuf;
    dynbuf = pvPortMalloc(UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAX_SIZE);
    if(uavcan_protocol_RestartNodeRequest_decode(transfer, 0, &reqrnr, &pdynbuf) >= 0)
    {
      if(reqrnr.magic_number == UAVCAN_PROTOCOL_RESTARTNODE_MAGIC_NUMBER)
      {
        rnr.ok = 1;
      }
    }
    canardReleaseRxTransferPayload(ins, transfer);


    const uint32_t size = uavcan_protocol_RestartNodeResponse_encode(&rnr, &_internal_buffer[0]);

    const int16_t resp_res = canardRequestOrRespond(ins,
                                                    transfer->source_node_id,
                                                    UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE,
                                                    UAVCAN_PROTOCOL_RESTARTNODE_ID,
                                                    &transfer->transfer_id,
                                                    transfer->priority,
                                                    CanardResponse,
                                                    &_internal_buffer[0],
                                                    (uint16_t)size);
    //Error happened. declare bad health
    if (resp_res <= 0)
    {
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    
    //free memory
    if(dynbuf != NULL)
    {
       vPortFree(dynbuf);
    }

    if(rnr.ok)
    {
      mcual_bootloader();
    }

    return;
  }

#ifdef CONFIG_LIBCOCOBOT_LOADER
  if(cocobot_loader_on_transfer_received(transfer))
  {
    return;
  }
#endif

  canardReleaseRxTransferPayload(ins, transfer);
}

static bool cocobot_can_should_accept_transfert(const CanardInstance* ins,
                                                uint64_t* out_data_type_signature,
                                                uint16_t data_type_id,
                                                CanardTransferType transfer_type,
                                                uint8_t source_node_id)
{
  (void)ins;
  //Accept node info
  if ((transfer_type == CanardTransferTypeRequest) &&
      (data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
  {
    *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE;
    return true;
  }
  
  //Accept restart node
  if ((transfer_type == CanardTransferTypeRequest) &&
      (data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
  {
    *out_data_type_signature = UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE;
    return true;
  }

  bool r = false;

#ifdef CONFIG_LIBCOCOBOT_LOADER
  if(r == false)
  {
    r = cocobot_loader_should_accept_transfer(out_data_type_signature,
                                              data_type_id,
                                              transfer_type,
                                              source_node_id);
  }
#endif

  return r;
}

void cocobot_can_task(void)
{
  for (;;)
  {
    uint16_t ticks = mcual_timer_get_value(CONFIG_LIBCOCOBOT_CAN_TIMER);
    uint16_t delta = ticks - _last_timer_ticks;
    _last_timer_ticks = ticks;
    _timestamp_ms += delta;

    //Transmit Tx queue
    for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&_canard)) != NULL;)
    {
      const int16_t tx_res = canardSTM32Transmit(txf);
      if (tx_res < 0)
      {
        // Failure - drop the frame
        canardPopTxQueue(&_canard);
        _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
      }
      else if (tx_res > 0)
      {
        // Success - just drop the frame
        canardPopTxQueue(&_canard);
      }
      else
      {
        // Timeout - try again later
        break;
      }
    }

    //Receiving
    CanardCANFrame rx_frame;
    const int16_t rx_res = canardSTM32Receive(&rx_frame);
    if (rx_res < 0)
    {
      // Failure - report
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    else if (rx_res > 0)
    {
      // Success - process the frame
      canardHandleRxFrame(&_canard, &rx_frame, _timestamp_ms * 1000);
    }

    /*
       const uint64_t ts = getMonotonicTimestampUSec();

       if (ts >= next_1hz_service_at)
       {
       next_1hz_service_at += 1000000;
       process1HzTasks(ts);
       }
       */
  }
}

void cocobot_can_init(void)
{
  // |°< coin coin !
  canardInit(&_canard, _canard_memory_pool, sizeof(_canard_memory_pool), cocobot_can_on_transfer_received, cocobot_can_should_accept_transfert, NULL);

	CanardSTM32CANTimings canbus_timings;
	canardSTM32ComputeCANTimings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 1000000, &canbus_timings);

	canardSTM32Init(&canbus_timings, CanardSTM32IfaceModeNormal);

#pragma message "TODO: read id from flash or eeprom"
	canardSetLocalNodeID(&_canard, 127);

  _last_timer_ticks = 0;
  _timestamp_ms = 0;
  mcual_timer_init(CONFIG_LIBCOCOBOT_CAN_TIMER, 1000);
}

#ifdef CONFIG_OS_USE_FREERTOS
static void cocobot_can_thread(void * arg)
{
  (void)arg;
  cocobot_can_run();
}

void cocobot_can_run(void)
{
  xTaskCreate(cocobot_can_thread, "can", 512, NULL, 1, NULL);
}
#endif

void cocobot_can_set_mode(uint8_t mode)
{
  _mode = mode;
}


int16_t cocobot_can_request_or_respond(uint8_t destination_node_id,
                                       uint64_t data_type_signature,
                                       uint8_t data_type_id,
                                       uint8_t* inout_transfer_id,
                                       uint8_t priority,
                                       CanardRequestResponse kind,
                                       const void* payload,
                                       uint16_t payload_len)
{
#pragma message "TODO: Add mutex for libcanard API access"
  return canardRequestOrRespond(&_canard,
                                destination_node_id,
                                data_type_signature,
                                data_type_id,
                                inout_transfer_id,
                                priority,
                                kind,
                                payload,
                                payload_len);
}

void cocobot_can_release_rx_transfer_payload(CanardRxTransfer* transfer)
{
   canardReleaseRxTransferPayload(&_canard, transfer);
}

//canard stm32 compat
int usleep(useconds_t usec)
{
#ifdef CONFIG_LIBCOCOBOT_LOADER
  volatile unsigned int i;
  for(i = 0; i < usec * 100; i += 1);
#else
  vTaskDelay((usec / 1000) / portTICK_PERIOD_MS);
#endif

  return 0;
}

#endif
