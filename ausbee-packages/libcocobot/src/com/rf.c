#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_COM_RF
#include <stdint.h>
#include <mcual.h>
#include <canard.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
#include <cocobot/rf.h>
#include <cocobot.h>

#define MAX_DELAY_SYNCHRO_us  1000000
#define TX_TIME_us               5000
#define TX_DEAD_TIME_us          1000

#define DEST_ID_FROM_ID(x)                          ((uint8_t) (((x) >> 8U)  & 0x7FU))
#define IS_NOT_BROADCAST(x)                         ((bool)    (((x) >> 7U)  & 0x1U))

#define COM_RF_TX_SIZE  64
#define COM_RF_RX_SIZE  64

typedef enum 
{
  COM_RF_CLIENT_PBRAIN,
  COM_RF_CLIENT_SBRAIN,
  COM_RF_CLIENT_EXP,
  COM_RF_CLIENT_PC1,
  COM_RF_CLIENT_PC2,

  COM_RF_CLIENT_ALL,
  COM_RF_CLIENT_NUMBER,
} com_rf_client_id;

typedef enum
{
  COM_RF_STATE_INIT,
  COM_RF_STATE_WAIT_SYNCHRO,
  COM_RF_STATE_BEGIN_TX,
  COM_RF_STATE_TX,
  COM_RF_STATE_RX,
} com_rf_state_t;

static QueueHandle_t _tx_queue[COM_RF_CLIENT_NUMBER];
static QueueHandle_t _rx_queue;
static com_rf_state_t _state;
static uint64_t _start_time_us;
static uint64_t _next_tx_time_us;
static uint8_t beacons;
static uint64_t _timestamp_us;
static SemaphoreHandle_t _mutex;

com_rf_client_id cocobot_com_rf_convert_id(uint8_t id)
{
  switch(id)
  {
    case 10:
      return COM_RF_CLIENT_PBRAIN;

    case 30:
      return COM_RF_CLIENT_SBRAIN;

    case 50:
      return COM_RF_CLIENT_EXP;

    case 110:
      return COM_RF_CLIENT_PC1;

    case 111:
      return COM_RF_CLIENT_PC2;
  }

  return COM_RF_CLIENT_ALL;
}

void cocobot_com_rf_handle_state(uint64_t timestamp_us)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);
  _timestamp_us = timestamp_us;
  xSemaphoreGive(_mutex);

  switch(_state)
  {
    case COM_RF_STATE_INIT:
      _state = COM_RF_STATE_WAIT_SYNCHRO;
      _start_time_us = timestamp_us;
      break;

    case COM_RF_STATE_WAIT_SYNCHRO:
      if(timestamp_us - _start_time_us > MAX_DELAY_SYNCHRO_us)
      {
        _state = COM_RF_STATE_BEGIN_TX;
      }
      break;

    case COM_RF_STATE_BEGIN_TX:
      _state = COM_RF_STATE_TX;
      _start_time_us = timestamp_us;
      //_next_tx_time_us = timestamp_us + TX_TIME_us * (COM_RF_CLIENT_NUMBER - 1);
      cocobot_com_rf_low_level_begin_tx();

      for(int i = 0; i < COM_RF_CLIENT_NUMBER - 1; i += 1)
      {
        if(beacons & (1 << i))
        {
          platform_led_set(1 << (PLATFORM_LED_GREEN_9_pos - i));
          platform_led_clear(1 << (PLATFORM_LED_RED_9_pos - i));
        }
        else
        {
          platform_led_clear(1 << (PLATFORM_LED_GREEN_9_pos - i));
          platform_led_set(1 << (PLATFORM_LED_RED_9_pos - i));
        }
      }
      beacons = (1 << cocobot_com_rf_convert_id(cocobot_com_get_node_id()));
      break;

    case COM_RF_STATE_TX:
      if(timestamp_us - _start_time_us >= (TX_TIME_us - TX_DEAD_TIME_us))
      {
        _state = COM_RF_STATE_RX;
        cocobot_com_rf_low_level_end_tx();
      }
      break;

    case COM_RF_STATE_RX:
      if(timestamp_us >= _next_tx_time_us)
      {
        _state = COM_RF_STATE_BEGIN_TX;
      }
      break;
  }
}

void cocobot_com_rf_init(void)
{
  unsigned int i;

  _mutex = xSemaphoreCreateMutex();
  for(i = 0; i < sizeof(_tx_queue)/sizeof(_tx_queue[0]); i += 1)
  {
    _tx_queue[i] = xQueueCreate(COM_RF_TX_SIZE, sizeof(CanardCANFrame));
  }
  _rx_queue = xQueueCreate(COM_RF_RX_SIZE, sizeof(CanardCANFrame));

  _state = COM_RF_STATE_INIT;

  cocobot_com_rf_low_level_init();
}

int16_t cocobot_com_rf_receive(CanardCANFrame* const frame, uint64_t timestamp_us)
{
  (void)frame;
  cocobot_com_rf_handle_state(timestamp_us);

  if(xQueueReceive(_rx_queue, frame, 0) == pdFALSE)
  {
    return 0;
  }
  else
  {
    return sizeof(CanardCANFrame);
  }
}

int16_t cocobot_com_rf_transmit(const CanardCANFrame* const frame, uint64_t timestamp_us)
{
  (void)timestamp_us;

  uint32_t id;
  if(IS_NOT_BROADCAST(frame->id)) 
  {
    id = DEST_ID_FROM_ID(frame->id);
  }
  else
  {
    id = 255;
  }

  if((id >= 10) && (id <= 29))
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_PBRAIN], frame, 0);
  }
  else if((id >= 30) && (id <= 49))
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_SBRAIN], frame, 0);
  }
  else if((id >= 50) && (id <= 59))
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_EXP], frame, 0);
  }
  else if((id >= 70) && (id <= 79))
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_PC1], frame, 0);
    xQueueSend(_tx_queue[COM_RF_CLIENT_PC2], frame, 0);
  }
  else if((id >= 110) && (id <= 120))
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_PC1], frame, 0);
    xQueueSend(_tx_queue[COM_RF_CLIENT_PC2], frame, 0);
  }
  else
  {
    xQueueSend(_tx_queue[COM_RF_CLIENT_ALL], frame, 0);
  }

  return sizeof(CanardCANFrame);
}

void cocobot_com_rf_recv(uint8_t src_id, int8_t rssi, com_rf_packet_t * p)
{
  (void)rssi;

  switch(p->type)
  {
    case COM_RF_PACKET_STATUS:
      {
        uprintf("T %lu\r\n", (uint32_t)(_timestamp_us & 0xFFFFFFFF));
        const com_rf_client_id clid = cocobot_com_rf_convert_id(src_id);
        const com_rf_client_id self_id = cocobot_com_rf_convert_id(cocobot_com_get_node_id());

        xSemaphoreTake(_mutex, portMAX_DELAY);
        if(self_id > clid)
        {
          _next_tx_time_us = _timestamp_us + TX_TIME_us * (self_id - clid);
        }
        else
        {
          _next_tx_time_us = _timestamp_us + TX_TIME_us * (COM_RF_CLIENT_NUMBER - 1 - (clid - self_id));
        }
        xSemaphoreGive(_mutex);

        platform_led_clear(PLATFORM_LED_GREEN_2);
        beacons |= (1 << clid);
      }
      break;

    case COM_RF_PACKET_DATA:
      xQueueSend(_rx_queue, &p->frame, 0);
      break;
  }
}

#endif
