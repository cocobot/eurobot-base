#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_COM

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
static uint8_t _canard_memory_pool[CONFIG_LIBCOCOBOT_COM_MEMORY_POOL_SIZE];
static uint8_t _internal_buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
static uint8_t _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
static uint8_t _mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION;
static uint16_t _last_timer_ticks;
static uint64_t _timestamp_ms;
static uint64_t _next_1hz_service_at;

#ifdef CONFIG_LIBCOCOBOT_COM_USART
#define COCOBOT_COM_USART_START 'C'
typedef struct 
{
  uint8_t start;
  uint8_t len;
  uint8_t data[8];
  uint16_t crc;
} cocobot_com_usart_frame_t;
#endif

static void cocobot_com_fill_status(uavcan_protocol_NodeStatus * ns)
{
  ns->uptime_sec = _timestamp_ms;
  ns->health = _health;
  ns->mode = _mode;
  ns->sub_mode = 0;
  ns->vendor_specific_status_code = 0;
}

static void cocobot_com_on_transfer_received(CanardInstance* ins,
                                             CanardRxTransfer* transfer)
{
  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
      (transfer->data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
  {
    canardReleaseRxTransferPayload(ins, transfer);

    uavcan_protocol_GetNodeInfoResponse nir; 
    cocobot_com_fill_status(&nir.status);
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

    cocobot_com_request_or_respond(transfer->source_node_id,
                                   UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
                                   UAVCAN_PROTOCOL_GETNODEINFO_ID,
                                   &transfer->transfer_id,
                                   transfer->priority,
                                   CanardResponse,
                                   &_internal_buffer[0],
                                   (uint16_t)size);

    return;
  }

  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
      (transfer->data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
  {
    uint8_t ok = 0;

    uavcan_protocol_RestartNodeRequest reqrnr; 
    uint8_t * pdynbuf = &_internal_buffer[0];
    if(uavcan_protocol_RestartNodeRequest_decode(transfer, 0, &reqrnr, &pdynbuf) >= 0)
    {
      if(reqrnr.magic_number == UAVCAN_PROTOCOL_RESTARTNODE_MAGIC_NUMBER)
      {
        ok = 1;
      }
    }
    canardReleaseRxTransferPayload(ins, transfer);


    uavcan_protocol_RestartNodeResponse rnr; 
    rnr.ok = ok; 
    const uint32_t size = uavcan_protocol_RestartNodeResponse_encode(&rnr, &_internal_buffer[0]);

    cocobot_com_request_or_respond(transfer->source_node_id,
                                   UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE,
                                   UAVCAN_PROTOCOL_RESTARTNODE_ID,
                                   &transfer->transfer_id,
                                   transfer->priority,
                                   CanardResponse,
                                   &_internal_buffer[0],
                                   (uint16_t)size);
    
    if(ok)
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

static bool cocobot_com_should_accept_transfert(const CanardInstance* ins,
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

#ifdef CONFIG_LIBCOCOBOT_COM_USART
int16_t cocobot_com_usart_transmit(const CanardCANFrame* const frame)
{
   platform_led_toggle(1 << 8);

  unsigned int i;
  cocobot_com_usart_frame_t uframe;

  uframe.start = COCOBOT_COM_USART_START;
  uframe.len = frame->data_len;
  for(i = 0; i < 8; i += 1)
  {
     uframe.data[i] = frame->data[i];
  }
  uframe.crc = 0x1310;

  uint8_t * ptr = (uint8_t *)&uframe;
  for(i = 0; i < sizeof(cocobot_com_usart_frame_t); i += 1, ptr += 1)
  {
    mcual_usart_send(PLATFORM_USART_USER, *ptr);
  }

  return sizeof(cocobot_com_usart_frame_t);
}
#endif

uint64_t cocobot_com_process_event(void)
{
  uint16_t ticks = mcual_timer_get_value(CONFIG_LIBCOCOBOT_COM_TIMER);
  uint16_t delta = ticks - _last_timer_ticks;
  _last_timer_ticks = ticks;
  _timestamp_ms += delta;

  //Transmit Tx queue
  for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&_canard)) != NULL;)
  {
#ifdef CONFIG_LIBCOCOBOT_COM_USART
    cocobot_com_usart_transmit(txf);
    canardPopTxQueue(&_canard);
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_CAN
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
#endif
  }

#ifdef CONFIG_LIBCOCOBOT_COM_CAN
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
#endif


  if (_timestamp_ms >= _next_1hz_service_at)
  {
    mcual_usart_send(PLATFORM_USART_USER, 'a');
    platform_led_toggle(1 << 10);
    _next_1hz_service_at += 1000;

    //clean up every seconds 
    canardCleanupStaleTransfers(&_canard, _timestamp_ms * 1000);
    mcual_usart_send(PLATFORM_USART_USER, '0');

    //send node info
    uavcan_protocol_NodeStatus ns;
    cocobot_com_fill_status(&ns);

    ///!\ do not remove static !!!
    static uint8_t transfer_id;

    const uint32_t size = uavcan_protocol_NodeStatus_encode(&ns, &_internal_buffer[0]);

    cocobot_com_broadcast(UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                          UAVCAN_PROTOCOL_NODESTATUS_ID,
                          &transfer_id,
                          CANARD_TRANSFER_PRIORITY_LOW,
                          &_internal_buffer[0],
                          size);
    mcual_usart_send(PLATFORM_USART_USER, '1');
  }

  return _timestamp_ms;
}

void cocobot_com_init(void)
{
  // |°< coin coin !
  canardInit(&_canard, _canard_memory_pool, sizeof(_canard_memory_pool), cocobot_com_on_transfer_received, cocobot_com_should_accept_transfert, NULL);

#ifdef CONFIG_LIBCOCOBOT_COM_CAN
	CanardSTM32CANTimings canbus_timings;
	canardSTM32ComputeCANTimings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 1000000, &canbus_timings);

	canardSTM32Init(&canbus_timings, CanardSTM32IfaceModeNormal);
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_USART
  mcual_usart_init(PLATFORM_USART_USER, 115200);
#endif

#pragma message "TODO: read id from flash or eeprom"
	canardSetLocalNodeID(&_canard, 127);

  _last_timer_ticks = 0;
  _timestamp_ms = 0;
  _next_1hz_service_at = 0;
  mcual_timer_init(CONFIG_LIBCOCOBOT_COM_TIMER, 1000);
}

#ifdef CONFIG_OS_USE_FREERTOS
static void cocobot_com_thread(void * arg)
{
  (void)arg;
  for(;;)
  {
    platform_led_toggle(1 << 9);
    cocobot_com_process_event();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void cocobot_com_run(void)
{
  xTaskCreate(cocobot_com_thread, "can", 512, NULL, 1, NULL);
}
#endif

void cocobot_com_set_mode(uint8_t mode)
{
  _mode = mode;
}


int16_t cocobot_com_request_or_respond(uint8_t destination_node_id,
                                       uint64_t data_type_signature,
                                       uint8_t data_type_id,
                                       uint8_t* inout_transfer_id,
                                       uint8_t priority,
                                       CanardRequestResponse kind,
                                       const void* payload,
                                       uint16_t payload_len)
{
#pragma message "TODO: Add mutex for libcanard API access"
  int16_t r = canardRequestOrRespond(&_canard,
                                     destination_node_id,
                                     data_type_signature,
                                     data_type_id,
                                     inout_transfer_id,
                                     priority,
                                     kind,
                                     payload,
                                     payload_len);
  if(r <= 0)
  {
    _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
  }

  return r;
}

int16_t cocobot_com_broadcast(uint64_t data_type_signature,
                              uint16_t data_type_id,
                              uint8_t* inout_transfer_id,
                              uint8_t priority,
                              const void* payload,
                              uint16_t payload_len)
{
#pragma message "TODO: Add mutex for libcanard API access"
  int16_t r = canardBroadcast(&_canard,
                              data_type_signature,
                              data_type_id,
                              inout_transfer_id,
                              priority,
                              payload,
                              payload_len);
  if(r <= 0)
  {
    _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
  }

  return r;
}

void cocobot_com_release_rx_transfer_payload(CanardRxTransfer* transfer)
{
   canardReleaseRxTransferPayload(&_canard, transfer);
}

//canard stm32 compat
int usleep(useconds_t usec)
{
#ifdef CONFIG_OS_USE_FREERTOS
  vTaskDelay((usec / 1000) / portTICK_PERIOD_MS);
#else
  volatile unsigned int i;
  for(i = 0; i < usec * 100; i += 1);
#endif

  return 0;
}

#endif
