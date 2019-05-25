#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_COM

#include <cocobot.h>
#include <mcual.h>
#include <platform.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef CONFIG_OS_USE_FREERTOS
# include <FreeRTOS.h>
# include <task.h>
# include <semphr.h>
#else
# include <malloc_wrapper.h>
#endif
#include <canard.h>
#include <canard_stm32.h>
#include <unistd.h>
#include "dsdl/uavcan/protocol/GetNodeInfo.h"
#include "dsdl/uavcan/protocol/RestartNode.h"
#include "dsdl/uavcan/protocol/NodeStatus.h"
#include "dsdl/uavcan/protocol/param/GetSet.h"
#include "dsdl/uavcan/protocol/debug/LogMessage.h"
#include <include/generated/git.h>
#include <cocobot/rf.h>

#undef DEBUG_FRAME

#define SOURCE_ID_FROM_ID(x)                        ((uint8_t) (((x) >> 0U)  & 0x7FU))
#define DEST_ID_FROM_ID(x)                          ((uint8_t) (((x) >> 8U)  & 0x7FU))

typedef enum 
{
  COCOBOT_COM_SOURCE_LIBCANARD,
  COCOBOT_COM_SOURCE_CAN,
  COCOBOT_COM_SOURCE_USART1,
  COCOBOT_COM_SOURCE_USART2,
  COCOBOT_COM_SOURCE_RF,
} cocobot_com_source_t;

static uint64_t const _canard_id = CONFIG_LIBCOCOBOT_COM_ID;
static CanardInstance _canard;
static uint8_t _canard_memory_pool[CONFIG_LIBCOCOBOT_COM_MEMORY_POOL_SIZE];
static uint8_t _internal_buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
static uint8_t _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
static uint8_t _mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION;
static uint32_t _last_timer_ticks;
static uint32_t _can_error;
static uint64_t _timestamp_us;
static uint64_t _next_1hz_service_at;
static uint8_t _printf_transfer_id = 0;
#ifdef CONFIG_OS_USE_FREERTOS
static SemaphoreHandle_t _mutex;
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_USART
#define COCOBOT_COM_USART_START 'C'
typedef struct __attribute__((__packed__))
{
  uint8_t start;
  uint32_t id;
  uint8_t len;
  uint8_t data[8];
  uint16_t crc;
} cocobot_com_usart_frame_t;
#endif

static void cocobot_com_fill_status(uavcan_protocol_NodeStatus * ns)
{
  ns->uptime_sec = _timestamp_us / 1000000;
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
    if(uavcan_protocol_RestartNodeRequest_decode(transfer, transfer->payload_len, &reqrnr, &pdynbuf) >= 0)
    {
      if(reqrnr.magic_number == UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAGIC_NUMBER)
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

  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
      (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID))
  {
    uavcan_protocol_param_GetSetRequest reqgsr; 
    uavcan_protocol_param_GetSetResponse gsr; 

    //set default response
    gsr.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY; 
    gsr.default_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY; 
    gsr.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY; 
    gsr.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY; 
    gsr.name.len = 0;

    uint8_t * pdynbuf = &_internal_buffer[0];
    uint8_t reboot = 0;
    if(uavcan_protocol_param_GetSetRequest_decode(transfer, transfer->payload_len, &reqgsr, &pdynbuf) >= 0)
    {
      if(strcmp((char *)reqgsr.name.data, "ID") == 0)
      {
#ifndef AUSBEE_SIM
        mcual_loader_flash_u64((uint32_t)&_canard_id, reqgsr.value.integer_value);
#endif
        gsr.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
        gsr.value.integer_value = reqgsr.value.integer_value;
        gsr.name.len = reqgsr.name.len;
        gsr.name.data = reqgsr.name.data;
        reboot = 1;
      }
    }
    canardReleaseRxTransferPayload(ins, transfer);


    const uint32_t size = uavcan_protocol_param_GetSetResponse_encode(&gsr, &_internal_buffer[0]);

    cocobot_com_request_or_respond(transfer->source_node_id,
                                   UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE,
                                   UAVCAN_PROTOCOL_PARAM_GETSET_ID,
                                   &transfer->transfer_id,
                                   transfer->priority,
                                   CanardResponse,
                                   &_internal_buffer[0],
                                   (uint16_t)size);
    
    if(reboot)
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

#ifdef CONFIG_LIBCOCOBOT_COM_USER_HANDLER
  if(com_on_transfer_received(transfer))
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
  (void)source_node_id;

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

  //Accept GetSet
  if ((transfer_type == CanardTransferTypeRequest) &&
      (data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID))
  {
    *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
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

#ifdef CONFIG_LIBCOCOBOT_COM_USER_HANDLER
  if(r == false)
  {
    r = com_should_accept_transfer(out_data_type_signature,
                                   data_type_id,
                                   transfer_type,
                                   source_node_id);
  }
#endif

  return r;
}

#ifdef CONFIG_LIBCOCOBOT_COM_USART
static uint16_t cocobot_com_crc16_update(uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;
  for(i = 0; i < 8; i += 1)
  {
    if (crc & 1)
    {
      crc = (crc >> 1) ^ 0xA001;
    }
    else
    {
      crc = (crc >> 1);
    }
  }

  return crc;
}

int16_t cocobot_com_usart_transmit(const CanardCANFrame* const frame)
{
  unsigned int i;
  cocobot_com_usart_frame_t uframe;

  uint16_t crc = 0xFFFF;

  uframe.start = COCOBOT_COM_USART_START;
  uframe.id = frame->id;
  uframe.len = frame->data_len;
  for(i = 0; i < 8; i += 1)
  {
     uframe.data[i] = frame->data[i];
  }

  uint8_t * ptr = (uint8_t *)&uframe;
  for(i = 0; i < sizeof(uframe) - 2; i += 1, ptr += 1)
  {
    crc = cocobot_com_crc16_update(crc, *ptr);
  }
  uframe.crc = crc;

  ptr = (uint8_t *)&uframe;
  for(i = 0; i < sizeof(cocobot_com_usart_frame_t); i += 1, ptr += 1)
  {
    mcual_usart_send(PLATFORM_USART_USER, *ptr);
  }

  return sizeof(cocobot_com_usart_frame_t);
}

#ifdef PLATFORM_USART_USER2
int16_t cocobot_com_usart2_transmit(const CanardCANFrame* const frame)
{
  unsigned int i;
  cocobot_com_usart_frame_t uframe;

  uint16_t crc = 0xFFFF;

  uframe.start = COCOBOT_COM_USART_START;
  uframe.id = frame->id;
  uframe.len = frame->data_len;
  for(i = 0; i < 8; i += 1)
  {
     uframe.data[i] = frame->data[i];
  }

  uint8_t * ptr = (uint8_t *)&uframe;
  for(i = 0; i < sizeof(uframe) - 2; i += 1, ptr += 1)
  {
    crc = cocobot_com_crc16_update(crc, *ptr);
  }
  uframe.crc = crc;

  ptr = (uint8_t *)&uframe;
  for(i = 0; i < sizeof(cocobot_com_usart_frame_t); i += 1, ptr += 1)
  {
    mcual_usart_send(PLATFORM_USART_USER2, *ptr);
  }

  return sizeof(cocobot_com_usart_frame_t);
}
#endif

int16_t cocobot_com_usart_receive(CanardCANFrame* const frame)
{
  static cocobot_com_usart_frame_t uframe;

  while(1)
  {
    int16_t byte = mcual_usart_recv_no_wait(PLATFORM_USART_USER);
    if(byte < 0)
    {
      return 0;
    }

    unsigned int i;
    uint8_t * ptr = (uint8_t *)&uframe;
    for(i = 0; i < sizeof(cocobot_com_usart_frame_t) - 1; i += 1)
    {
      *ptr = *(ptr + 1);
      ptr += 1;
    }
    *ptr = byte & 0xFF;

    if(uframe.start == COCOBOT_COM_USART_START)
    {
      uint16_t crc = 0xFFFF;
      uint8_t * ptr = (uint8_t *)&uframe;
      for(i = 0; i < sizeof(cocobot_com_usart_frame_t) - 2; i += 1, ptr += 1)
      {
        crc = cocobot_com_crc16_update(crc, *ptr);
      }
      if(crc == uframe.crc)
      {
        frame->id = uframe.id;
        frame->data_len = uframe.len;
        for(i = 0; i < 8; i += 1)
        {
          frame->data[i] = uframe.data[i];
        }
        return 1;
      }
    }
  }
}

#ifdef PLATFORM_USART_USER2
int16_t cocobot_com_usart2_receive(CanardCANFrame* const frame)
{
  static cocobot_com_usart_frame_t uframe;

  while(1)
  {
    int16_t byte = mcual_usart_recv_no_wait(PLATFORM_USART_USER2);
    if(byte < 0)
    {
      return 0;
    }

    unsigned int i;
    uint8_t * ptr = (uint8_t *)&uframe;
    for(i = 0; i < sizeof(cocobot_com_usart_frame_t) - 1; i += 1)
    {
      *ptr = *(ptr + 1);
      ptr += 1;
    }
    *ptr = byte & 0xFF;

    if(uframe.start == COCOBOT_COM_USART_START)
    {
      uint16_t crc = 0xFFFF;
      uint8_t * ptr = (uint8_t *)&uframe;
      for(i = 0; i < sizeof(cocobot_com_usart_frame_t) - 2; i += 1, ptr += 1)
      {
        crc = cocobot_com_crc16_update(crc, *ptr);
      }
      if(crc == uframe.crc)
      {
        frame->id = uframe.id;
        frame->data_len = uframe.len;
        for(i = 0; i < 8; i += 1)
        {
          frame->data[i] = uframe.data[i];
        }
        return 1;
      }
    }
  }
}
#endif
#endif

void cocobot_com_retransmit(const CanardCANFrame * rx_frame, cocobot_com_source_t source)
{
  (void)rx_frame;
  (void)source;
#ifdef DEBUG_FRAME
 //if(SOURCE_ID_FROM_ID(rx_frame->id) == 72)
 //uprintf("[TRANSMIT] %lu/%d(%d) -- %d->%d -- %d %d %d %d %d %d %d %d\r\n",
 //          rx_frame->id,
 //          rx_frame->data_len,
 //          source,
 //          SOURCE_ID_FROM_ID(rx_frame->id),
 //          DEST_ID_FROM_ID(rx_frame->id),
 //          rx_frame->data[0],
 //          rx_frame->data[1],
 //          rx_frame->data[2],
 //          rx_frame->data[3],
 //          rx_frame->data[4],
 //          rx_frame->data[5],
 //          rx_frame->data[6],
 //          rx_frame->data[7]);
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_CAN
 if(source != COCOBOT_COM_SOURCE_CAN)
 {
    mcual_can_transmit(rx_frame);
 }
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_USART
 if(source != COCOBOT_COM_SOURCE_USART1)
 {
   cocobot_com_usart_transmit(rx_frame);
 }

#ifdef PLATFORM_USART_USER2
 if(source != COCOBOT_COM_SOURCE_USART2)
 {
   cocobot_com_usart2_transmit(rx_frame);
 }
#endif
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_RF
 if(source != COCOBOT_COM_SOURCE_RF)
 {
   cocobot_com_rf_transmit(rx_frame, _timestamp_us);
 }
#endif
}

static void cocobot_com_transmit_tx_queue(void)
{
  for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&_canard)) != NULL;)
  {
    cocobot_com_retransmit(txf, COCOBOT_COM_SOURCE_LIBCANARD);
    canardPopTxQueue(&_canard);
  }
}

void cocobot_com_flush(void)
{
  cocobot_com_transmit_tx_queue();

  //TODO: flush can
}

uint64_t cocobot_com_process_event(void)
{
  uint32_t ticks = mcual_timer_get_value(CONFIG_LIBCOCOBOT_COM_TIMER);
#if CONFIG_LIBCOCOBOT_COM_TIMER == 1
  uint16_t delta = ticks - _last_timer_ticks;
#else
  uint32_t delta = ticks - _last_timer_ticks;
#endif
  _last_timer_ticks = ticks;
  
  _timestamp_us += ((uint64_t)delta);


  //Transmit Tx queue
  cocobot_com_transmit_tx_queue();

  CanardCANFrame rx_frame;
  int16_t rx_res;
  (void)rx_res;
  (void)rx_frame;

#ifdef CONFIG_LIBCOCOBOT_COM_CAN
  rx_res = 1;
  while(rx_res > 0)
  {
    rx_res = mcual_can_recv_no_wait(&rx_frame);
    if (rx_res < 0)
    {
      // Failure - report
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    else if (rx_res > 0)
    {
      cocobot_com_retransmit(&rx_frame, COCOBOT_COM_SOURCE_CAN);
      canardHandleRxFrame(&_canard, &rx_frame, _timestamp_us);
    }
  }
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_USART
  rx_res = 1;
  while(rx_res > 0)
  {
    rx_res = cocobot_com_usart_receive(&rx_frame);
    if (rx_res < 0)
    {
      // Failure - report
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    else if (rx_res > 0)
    {
      cocobot_com_retransmit(&rx_frame, COCOBOT_COM_SOURCE_USART1);
      canardHandleRxFrame(&_canard, &rx_frame, _timestamp_us);
    }
  }
#endif

#ifdef PLATFORM_USART_USER2
  rx_res = 1;
  while(rx_res > 0)
  {
    rx_res = cocobot_com_usart2_receive(&rx_frame);
    if (rx_res < 0)
    {
      // Failure - report
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    else if (rx_res > 0)
    {
      cocobot_com_retransmit(&rx_frame, COCOBOT_COM_SOURCE_USART2);
      canardHandleRxFrame(&_canard, &rx_frame, _timestamp_us);
    }
  }
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_RF
  rx_res = 1;
  while(rx_res > 0)
  {
    rx_res = cocobot_com_rf_receive(&rx_frame, _timestamp_us);
    if (rx_res < 0)
    {
      // Failure - report
      _health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING;
    }
    else if (rx_res > 0)
    {
      cocobot_com_retransmit(&rx_frame, COCOBOT_COM_SOURCE_RF);
      canardHandleRxFrame(&_canard, &rx_frame, _timestamp_us);
    }
  }
#endif

#ifdef CONFIG_LIBCOCOBOT_POSITION 
  cocobot_position_com_async(_timestamp_us);
#endif
 #ifdef CONFIG_LIBCOCOBOT_GAME_STATE 
  cocobot_game_state_com_async(_timestamp_us);
#endif
  
  if (_timestamp_us >= _next_1hz_service_at)
  {
    platform_led_toggle(1);
    _next_1hz_service_at = _timestamp_us + 1000000;

    //clean up every seconds 
    canardCleanupStaleTransfers(&_canard, _timestamp_us);

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
  }

  return _timestamp_us;
}

void cocobot_com_init(void)
{
  // |°< coin coin !
  canardInit(&_canard, _canard_memory_pool, sizeof(_canard_memory_pool), cocobot_com_on_transfer_received, cocobot_com_should_accept_transfert, NULL);

#ifdef CONFIG_LIBCOCOBOT_COM_USART
  mcual_usart_init(PLATFORM_USART_USER, 115200);
#endif

#ifdef CONFIG_LIBCOCOBOT_COM_RF
  cocobot_com_rf_init();
#endif

#ifdef CONFIG_OS_USE_FREERTOS
  //create mutex
  _mutex = xSemaphoreCreateMutex();
#endif

#ifndef CONFIG_OS_USE_FREERTOS
#ifdef CONFIG_LIBCOCOBOT_COM_CAN
  //RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
	mcual_can_timings canbus_timings;
	mcual_can_compute_timings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 100000, &canbus_timings);

	mcual_can_init(&canbus_timings, mcualCanIfaceModeNormal);
#endif
#endif

  volatile uint64_t * ptr = (uint64_t *)&_canard_id;
	canardSetLocalNodeID(&_canard, *ptr & 0x7F);

  _last_timer_ticks = 0;
  _timestamp_us = 0;
  _next_1hz_service_at = 0;
  _can_error = 0;
  mcual_timer_init(CONFIG_LIBCOCOBOT_COM_TIMER, -1000000);
}

#ifdef CONFIG_OS_USE_FREERTOS
static void cocobot_com_thread(void * arg)
{
#ifdef CONFIG_LIBCOCOBOT_COM_CAN
  //RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
	mcual_can_timings canbus_timings;
#ifndef AUSBEE_SIM
	mcual_can_compute_timings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 100000, &canbus_timings);
#endif

	mcual_can_init(&canbus_timings, mcualCanIfaceModeNormal);
#endif

  (void)arg;
  for(;;)
  {
    cocobot_com_process_event();
#ifdef AUSBEE_SIM
    vTaskDelay(20 / portTICK_PERIOD_MS);
#endif
  }
}

void cocobot_com_run(void)
{
  xTaskCreate(cocobot_com_thread, "can", 1024, NULL, 1, NULL);
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
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreTake(_mutex, portMAX_DELAY);
#endif
  int16_t r = canardRequestOrRespond(&_canard,
                                     destination_node_id,
                                     data_type_signature,
                                     data_type_id,
                                     inout_transfer_id,
                                     priority,
                                     kind,
                                     payload,
                                     payload_len);
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreGive(_mutex);
#endif
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
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreTake(_mutex, portMAX_DELAY);
#endif
  int16_t r = canardBroadcast(&_canard,
                              data_type_signature,
                              data_type_id,
                              inout_transfer_id,
                              priority,
                              payload,
                              payload_len);
#ifdef CONFIG_OS_USE_FREERTOS
  xSemaphoreGive(_mutex);
#endif
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

void cocobot_com_printf(uint8_t level, char * fmt, ...)
{
  uavcan_protocol_debug_LogMessage msg;
  va_list ap;

  msg.level.value = level;

  msg.source.len = 0;
  msg.source.data = (uint8_t *)"";

  msg.text.data = pvPortMalloc(UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_TEXT_MAX_LENGTH);

  if(msg.text.data != NULL)
  {
    va_start(ap, fmt);
    msg.text.len = vsnprintf((char *)msg.text.data, UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_TEXT_MAX_LENGTH, fmt, ap);
    va_end(ap);

    void * buf = pvPortMalloc(UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_MAX_SIZE); 
    if(buf != NULL) 
    {
      const uint32_t size = uavcan_protocol_debug_LogMessage_encode(&msg, &_internal_buffer[0]);

      cocobot_com_broadcast(UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_SIGNATURE,
                            UAVCAN_PROTOCOL_DEBUG_LOGMESSAGE_ID,
                            &_printf_transfer_id,
                            CANARD_TRANSFER_PRIORITY_LOW,
                            &_internal_buffer[0],
                            size);

      vPortFree(buf);
    }

    vPortFree(msg.text.data);
  }
}

//canard stm32 compat
#ifndef AUSBEE_SIM
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

#endif
