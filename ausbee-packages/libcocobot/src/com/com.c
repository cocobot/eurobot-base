#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <mcual.h>

#define PING_COUNTER_CONFIG 10
#define COCOBOT_COM_HEADER_START 0xC0
#define INVALID_PTR ((void *)0xFFFFFFFF)

#define COCOBOT_COM_CAN_MAX_RECV_QUEUE 5

#define COM_ERROR_NO_QUEUE_AVAILABLE 0

typedef struct __attribute__((packed)) 
{
  uint8_t start;
  uint8_t src;
  uint16_t pid;
  uint16_t len;
  uint16_t crc;
} cocobot_com_header_t;

typedef struct __attribute__((packed)) 
{
  uint16_t pid;
  uint16_t len;
} cocobot_com_can_header_t;

typedef struct
{
  uint16_t source;
  uint32_t len;
  uint16_t pid;
  uint8_t * data;
} cocobot_com_can_recv_t;

static SemaphoreHandle_t _mutex;
static SemaphoreHandle_t _exec_mutex;
static mcual_usart_id_t _usart;
static char _printf_buffer[64];
static cocobot_com_handler_t _user_handler;
static uint8_t _can_buffer[8];
static uint8_t _can_counter;
static uint16_t _can_packet_counter;
static uint8_t _com_id;
static cocobot_com_can_recv_t recv_queues[COCOBOT_COM_CAN_MAX_RECV_QUEUE];
static uint8_t _last_src;

static void cocobot_com_can_flush(uint8_t id)
{
  if(_can_counter > 0)
  {
    mcual_can_frame_t frame;
    frame.id = (_can_packet_counter & 0xFFFF) | ((id & 0x1F) << 16);
    frame.data_len = _can_counter;
    memcpy(frame.data, _can_buffer, sizeof(frame.data));
    mcual_can_transmit(&frame);
    _can_counter = 0;
    _can_packet_counter += 1;
  }
}

static void cocobot_com_can_send(uint8_t id, uint8_t data)
{
  _can_buffer[_can_counter] = data; 
  _can_counter += 1;
  if(_can_counter == sizeof(_can_buffer))
  {
    cocobot_com_can_flush(id);
  }
}

static void cocobot_com_can_prepare(const cocobot_com_can_header_t * const header)
{
  uint8_t * ptr;

  _can_packet_counter = 0;

  //fill buffer with header data
  for(_can_counter = 0, ptr = (uint8_t *)header; _can_counter < sizeof(cocobot_com_can_header_t); _can_counter += 1, ptr += 1)
  {
    _can_buffer[_can_counter] = *ptr; 
  }

}

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

void cocobot_com_async_thread(void *arg)
{
  (void)arg;

  int ping_counter = 0;

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while(pdTRUE)
  {
    //send debug information if needed
#ifdef CONFIG_LIBCOCOBOT_ASSERV
    cocobot_position_handle_async_com();
#endif
#ifdef CONFIG_LIBCOCOBOT_ASSERV
    cocobot_asserv_handle_async_com();
#endif
//#ifdef CONFIG_LIBCOCOBOT_TRAJECTORY
//    cocobot_trajectory_handle_async_com();
//#endif
#ifdef CONFIG_LIBCOCOBOT_PATHFINDER
    cocobot_pathfinder_handle_async_com();
#endif
#ifdef CONFIG_LIBCOCOBOT_GAME_STATE
    cocobot_game_state_handle_async_com();
#endif
//#ifdef CONFIG_LIBCOCOBOT_ACTION_SCHEDULER
//    cocobot_action_scheduler_handle_async_com();
//#endif

    if(ping_counter > PING_COUNTER_CONFIG)
    {
#ifdef AUSBEE_SIM
      //heartbeat PING
      fprintf(stderr, "PING");
#endif
      cocobot_com_send(COCOBOT_COM_PING_PID, "");
      ping_counter = 0;
    }
    else
    {
      ping_counter += 1;
    }

    //wait 100ms (minus time used by previous handler)
    vTaskDelayUntil( &xLastWakeTime, 100 / portTICK_PERIOD_MS);
  }
}

void cocobot_com_handle_packet(uint8_t src, uint16_t pid, uint8_t * data, uint16_t len)
{
  xSemaphoreTake(_exec_mutex, portMAX_DELAY);
  _last_src = src;
  switch(pid)
  {
#ifndef CONFIG_LIBCOCOBOT_LOADER
    case COCOBOT_COM_RESET_PID:
      if(len >= 1) {
        uint8_t id;
        cocobot_com_read_B(data, len, 0, &id);

        //start bootloader
        if(id == COCOBOT_COM_ID)
        {
          mcual_bootloader();
        }
      }
      break;
#endif
  }

#ifdef CONFIG_LIBCOCOBOT_LOADER
  cocobot_loader_handle_packet(pid, data, len);
#endif
#ifdef CONFIG_LIBCOCOBOT_TRAJECTORY
  cocobot_asserv_handle_sync_com(pid, data, len);
#endif
#ifdef CONFIG_LIBCOCOBOT_ASSERV
  cocobot_position_handle_sync_com(pid, data, len);
#endif
#ifdef CONFIG_LIBCOCOBOT_GAME_STATE
  cocobot_game_state_handle_sync_com(pid, data, len);
#endif

  if(_user_handler != NULL)
  {
    _user_handler(pid, data, len);
  }
  xSemaphoreGive(_exec_mutex);
}

uint8_t cocobot_com_get_src(void)
{
  return _last_src;
}

void cocobot_com_can_thread(void *arg)
{
  (void)arg;

  uint32_t i;

#ifdef CONFIG_OS_USE_FREERTOS
	mcual_can_timings canbus_timings;
	mcual_can_compute_timings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 1000000, &canbus_timings);
	mcual_can_init(&canbus_timings, mcualCanIfaceModeNormal);
#endif

  while(pdTRUE)
  {
    mcual_can_frame_t frame;
    mcual_can_recv(&frame);

    int src = (frame.id >> 16) & 0x1F;
    int packet_counter = frame.id & 0xFFFF;

    cocobot_com_can_recv_t * queue = NULL;

    //search already allocated queue
    for(i = 0; i < sizeof(recv_queues)/sizeof(recv_queues[0]); i += 1)
    {
      if(recv_queues[i].source == src)
      {
        queue = &recv_queues[0];
      }
    }

    if(queue == NULL)
    {
      //search free queue
      for(i = 0; i < sizeof(recv_queues)/sizeof(recv_queues[0]); i += 1)
      {
        if(recv_queues[i].source == 0)
        {
          queue = &recv_queues[0];
          queue->source = src;
        }
      }
    }

    if(queue == NULL)
    {
      //bad configuration ?
      cocobot_com_send(COCOBOT_COM_ERROR_PID, "B", COM_ERROR_NO_QUEUE_AVAILABLE);
    }
    else
    {
      uint32_t offset = 0;
      if(packet_counter == 0)
      {
        //deleted previous data
        if(queue->data != NULL)
        {
          vPortFree(queue->data);
        }

        uint32_t pid = frame.data[0] | (frame.data[1] << 8);
        uint32_t len = frame.data[2] | (frame.data[3] << 8);

        offset = 4;
        queue->pid = pid;
        queue->len = 2 + len;
        queue->data = pvPortMalloc(queue->len);
        memset(queue->data, 0, queue->len);
      }

      if(queue->data == NULL)
      {
        //bad synchro
        continue;
      }

      for(; offset < frame.data_len; offset += 1)
      {
        uint32_t data_offset = offset + packet_counter * 8 - 4;
        queue->data[data_offset] = frame.data[offset];
      }

      //check CRC
      uint16_t crc = 0xFFFF;
      for(i = 0; i < queue->len - 2; i += 1) 
      {
        crc = cocobot_com_crc16_update(crc, queue->data[i]);
      }
      
      uint16_t recv_crc = queue->data[i] | (queue->data[i + 1] << 8);

      if(crc == recv_crc)
      {
        cocobot_com_handle_packet(src, queue->pid, queue->data, queue->len - 2);

        //send data to UART
        cocobot_com_header_t header;
        header.start = COCOBOT_COM_HEADER_START;
        header.pid = queue->pid;
        header.src = src;
        header.crc = 0xFFFF;
        header.len = queue->len - 2;
        uint8_t * ptr = (uint8_t *)&header;
        size_t i;
        for(i = 0; i < sizeof(header) - 2; i += 1)
        {
          header.crc = cocobot_com_crc16_update(header.crc, *ptr);
          ptr += 1;
        }
        xSemaphoreTake(_mutex, portMAX_DELAY);
        ptr = (uint8_t *)&header;
        for(i = 0; i < sizeof(header); i += 1)
        {
          mcual_usart_send(_usart, *ptr);
          ptr += 1;
        }
        for(i = 0; i < queue->len; i += 1)
        {
          mcual_usart_send(_usart, queue->data[i]);
        }
        xSemaphoreGive(_mutex);


        vPortFree(queue->data);
        queue->data = NULL;
      }
    }
  }
}

void cocobot_com_uart_thread(void *arg)
{
  (void)arg;

  cocobot_com_header_t header;
  uint8_t * ptr = (uint8_t *)&header;
  unsigned int i;
  for(i = 0; i < sizeof(header); i += 1) 
  {
    ptr[i] = 0;
  }
  while(pdTRUE)
  {
    //get header
    uint8_t recv = mcual_usart_recv(_usart);
    for(i = 1; i < sizeof(header); i += 1) 
    {
      ptr[i - 1] = ptr[i];
    }
    ptr[sizeof(header) - 1] = recv;

    if(header.start == COCOBOT_COM_HEADER_START)
    {
      uint16_t crc = 0xFFFF;
      for(i = 0; i < sizeof(header) - 2; i += 1) 
      {
        crc = cocobot_com_crc16_update(crc, ptr[i]);
      }
      if(crc == header.crc)
      {
        uint8_t * data = pvPortMalloc(header.len);
        if((data != NULL) || (header.len == 0))
        {
          //TODO:: read DATA timeout and check CRCDATA
          for(i = 0; i < header.len; i += 1) 
          {
            data[i] = mcual_usart_recv(_usart);
          }

          //CRC
          uint16_t recv_crc = 0;
          recv_crc |= mcual_usart_recv(_usart);
          recv_crc |= (mcual_usart_recv(_usart) << 8);

          cocobot_com_handle_packet(header.src, header.pid, data, header.len);

          //send com data to CAN bus
          cocobot_com_can_header_t can_header;
          can_header.len = header.len;
          can_header.pid = header.pid;

          xSemaphoreTake(_mutex, portMAX_DELAY);
          cocobot_com_can_prepare(&can_header);
          for(i = 0; i < header.len; i += 1)
          {
            cocobot_com_can_send(header.src, data[i]);
          }
          cocobot_com_can_send(header.src, recv_crc & 0xFF);
          cocobot_com_can_send(header.src, (recv_crc >> 8) & 0xFF);
          cocobot_com_can_flush(header.src);
          xSemaphoreGive(_mutex);

          for(i = 0; i < sizeof(header); i += 1) 
          {
            ptr[i] = 0;
          }
          vPortFree(data);
        }
        else
        {
          for(i = 0; i < sizeof(header); i += 1) 
          {
            ptr[i] = 0;
          }
        }
      }
    }
  }
}

void cocobot_com_init(mcual_usart_id_t usart_id, unsigned int priority_monitor, unsigned int priority_async, cocobot_com_handler_t handler)
{
  _usart = usart_id;
  //create mutex
  _mutex = xSemaphoreCreateMutex();
  _exec_mutex = xSemaphoreCreateMutex();
  _user_handler = handler;
  _com_id = COCOBOT_COM_ID;

  //init usart peripheral
  mcual_usart_init(_usart, 1000000);

#ifndef CONFIG_OS_USE_FREERTOS
	mcual_can_timings canbus_timings;
	mcual_can_compute_timings(mcual_clock_get_frequency_Hz(MCUAL_CLOCK_PERIPHERAL_1), 1000000, &canbus_timings);
	mcual_can_init(&canbus_timings, mcualCanIfaceModeNormal);
#endif

  //start tasks
  xTaskCreate(cocobot_com_uart_thread, "com uart", 512, NULL, priority_monitor, NULL );
  xTaskCreate(cocobot_com_can_thread, "com can", 512, NULL, priority_monitor, NULL );
  xTaskCreate(cocobot_com_async_thread, "com. async", 512, NULL, priority_async, NULL );

}

static int cocobot_com_compute_len(char ** fmt, va_list ap, uint8_t * ptr)
{
  int size = 0; 
  int nested = 0;

  for(; (**fmt) != 0; (*fmt) += 1)
  {
    if(nested) 
    {
      if(**fmt == '[') 
      {
         nested += 1;
      }
      if(**fmt == ']') 
      {
        nested -= 1;
      }
    }

    if(nested == 0)
    {
      switch(**fmt)
      { 
        case 'F':
          {
            size += 4;
            if(ptr == NULL)
            {
              va_arg(ap, double);
            }
            else
            {
              va_arg(ap, size_t); //offsetof
            }
          }
          break;

        case 'B':
          {
            size += 1;
            if(ptr == NULL)
            {
              va_arg(ap, int);
            }
            else
            {
              va_arg(ap, size_t); //offsetof
            }
          }
          break;

        case 'H':
          {
            size += 2;
            if(ptr == NULL)
            {
              va_arg(ap, int);
            }
            else
            {
              va_arg(ap, size_t); //offsetof
            }
          }
          break;

        case 'D':
          {
            size += 4;
            if(ptr == NULL)
            {
              va_arg(ap, int);
            }
            else
            {
              va_arg(ap, size_t); //offsetof
            }
          }
          break;

        case 'S':
          {
            size += 2;
            if(ptr == NULL)
            {
              const char * v = va_arg(ap, const char *);
              size += strlen(v) + 1;
            }
            else
            {
              int offset = va_arg(ap, size_t); //offsetof
              if(ptr == INVALID_PTR)
              {
                continue;
              }
              const char * v = (char *)(ptr + offset);
              size += strlen(v) + 1;
            }
          }
          break;


        case ']':
          return size;
          break;

        case '[':
          {
            nested += 1;

            uint8_t * ptr = va_arg(ap, uint8_t *); //ptr
            int elmsize = va_arg(ap, size_t); //size
            int arrsize = va_arg(ap, size_t); //size
            unsigned int start = va_arg(ap, unsigned int); //start
            unsigned int end = va_arg(ap, unsigned int); //end

            size += 2;
            (*fmt) += 1;

            unsigned int i;
            while(end < start) {
              end += arrsize;
            }
            for(i = start; i != end; i += 1)
            {
              va_list ap_cpy;
              va_copy(ap_cpy, ap);
              char * nfmt = *fmt;
              if(arrsize != 0)
              {
                size += cocobot_com_compute_len(&nfmt, ap_cpy, ptr + elmsize * (i % arrsize));  
              }
              va_end(ap_cpy);
            }
          }
          break;

        default:
#ifdef AUSBEE_SIM
          fprintf(stderr, "COM: Unknown size of %c(%s)\n", **fmt, *fmt);
#endif
          break;
      }
    }
  }

  return size;
}

static uint16_t cocobot_com_send_data(char ** fmt, va_list ap, uint16_t crc, uint8_t * ptr)
{
  for(; (**fmt) != 0; (*fmt) += 1)
  {
    switch(**fmt)
    {
      case 'F':
        {
          float v = NAN;
          if(ptr == NULL)
          {
            v = va_arg(ap, double);
          
          }
          else
          {
            uint8_t * p = (uint8_t *)&v;
            int offset = va_arg(ap, size_t); //offsetof
            if(ptr == INVALID_PTR)
            {
              continue;
            }
            p[0] = ptr[offset + 0];
            p[1] = ptr[offset + 1];
            p[2] = ptr[offset + 2];
            p[3] = ptr[offset + 3];
          }

          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          cocobot_com_can_send(_com_id, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));
          mcual_usart_send(_usart, *(p + 2));
          cocobot_com_can_send(_com_id, *(p + 2));
          crc = cocobot_com_crc16_update(crc, *(p + 2));
          mcual_usart_send(_usart, *(p + 3));
          cocobot_com_can_send(_com_id, *(p + 3));
          crc = cocobot_com_crc16_update(crc, *(p + 3));
        }
        break;

      case 'B':
        {
          unsigned int v = 0;
          if(ptr == NULL)
          {
            v = va_arg(ap, int);
          }
          else
          {
            uint8_t * p = (uint8_t *)&v;
            int offset = va_arg(ap, size_t); //offsetof
            if(ptr == INVALID_PTR)
            {
              continue;
            }
            p[0] = ptr[offset + 0];
          }

          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
        }
        break;

      case 'H':
        {
          unsigned int v = 0;
          if(ptr == NULL)
          {
            v = va_arg(ap, int);
          }
          else
          {
            uint8_t * p = (uint8_t *)&v;
            int offset = va_arg(ap, size_t); //offsetof
            if(ptr == INVALID_PTR)
            {
              continue;
            }
            p[0] = ptr[offset + 0];
            p[1] = ptr[offset + 1];
          }

          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          cocobot_com_can_send(_com_id, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));
        }
        break;

      case 'D':
        {
          unsigned int v = 0;
          if(ptr == NULL)
          {
            v = va_arg(ap, int);
          }
          else
          {
            uint8_t * p = (uint8_t *)&v;
            int offset = va_arg(ap, size_t); //offsetof
            if(ptr == INVALID_PTR)
            {
              continue;
            }
            p[0] = ptr[offset + 0];
            p[1] = ptr[offset + 1];
            p[2] = ptr[offset + 2];
            p[3] = ptr[offset + 3];
          }

          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          cocobot_com_can_send(_com_id, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));
          mcual_usart_send(_usart, *(p + 2));
          cocobot_com_can_send(_com_id, *(p + 2));
          crc = cocobot_com_crc16_update(crc, *(p + 2));
          mcual_usart_send(_usart, *(p + 3));
          cocobot_com_can_send(_com_id, *(p + 3));
          crc = cocobot_com_crc16_update(crc, *(p + 3));
        }
        break;

      case 'S':
        {
          char * v = "";
          if(ptr == NULL)
          {
            v = va_arg(ap, char *);
          }
          else
          {
            int offset = va_arg(ap, size_t); //offsetof
            if(ptr == INVALID_PTR)
            {
              continue;
            }
            v = (char *)(ptr + offset);
          }

          unsigned int len = strlen(v);
          uint8_t * p = (uint8_t *)&len;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          cocobot_com_can_send(_com_id, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));


          p = (uint8_t *)v;
          unsigned int i;
          for(i = 0; i < strlen(v); i += 1)
          {
            mcual_usart_send(_usart, *(p + i));
            cocobot_com_can_send(_com_id, *(p + i));
            crc = cocobot_com_crc16_update(crc, *(p + i));
          }

          mcual_usart_send(_usart, 0);
          cocobot_com_can_send(_com_id, 0);
          crc = cocobot_com_crc16_update(crc, 0);
        }
        break;


      case ']':
        return crc;
        break;

      case '[':
        {
          uint8_t * ptr = va_arg(ap, uint8_t *); //ptr
          int elmsize = va_arg(ap, size_t); //size
          int arrsize = va_arg(ap, size_t); //size
          unsigned int start = va_arg(ap, unsigned int); //start
          unsigned int end = va_arg(ap, unsigned int); //end

          unsigned int i;
          (*fmt) += 1;
          while(end < start) {
            end += arrsize;
          }

          uint16_t v = end - start;
          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          cocobot_com_can_send(_com_id, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          cocobot_com_can_send(_com_id, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));

 
          for(i = start; i != end; i += 1)
          {
            va_list ap_cpy;
            va_copy(ap_cpy, ap);
            char * nfmt = *fmt;
            if(arrsize != 0)
            {      
              crc = cocobot_com_send_data(&nfmt, ap_cpy, crc, ptr + elmsize * (i % arrsize));
            }
            va_end(ap_cpy);
          }
          
          cocobot_com_send_data(fmt, ap, crc, INVALID_PTR);
        }
        break;

      default:
#ifdef AUSBEE_SIM
        fprintf(stderr, "COM: Unknown way to send data of %c(%s)\n", **fmt, *fmt);
#endif
        break;
    }
  }

  return crc;
}

void cocobot_com_send(uint16_t pid, char * fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  cocobot_com_header_t header;
  cocobot_com_can_header_t can_header;

  header.start = COCOBOT_COM_HEADER_START;
  header.pid = pid;
  header.src = _com_id;

  va_list ap_cpy_len;
  va_copy(ap_cpy_len, ap);
  char * pfmt = fmt;
  header.len = cocobot_com_compute_len(&pfmt, ap_cpy_len, NULL);
  va_end(ap_cpy_len);

  header.crc = 0xFFFF;
  uint8_t * ptr = (uint8_t *)&header;
  size_t i;
  for(i = 0; i < sizeof(header) - 2; i += 1)
  {
    header.crc = cocobot_com_crc16_update(header.crc, *ptr);
    ptr += 1;
  }

  //copy into can header
  can_header.pid = header.pid;
  can_header.len = header.len;

  xSemaphoreTake(_mutex, portMAX_DELAY);
  ptr = (uint8_t *)&header;
  for(i = 0; i < sizeof(header); i += 1)
  {
    mcual_usart_send(_usart, *ptr);
    ptr += 1;
  }

  cocobot_com_can_prepare(&can_header);

  uint16_t crc = 0xFFFF;

  pfmt = fmt;
  crc = cocobot_com_send_data(&pfmt, ap, crc, 0);
  va_end(ap);

  mcual_usart_send(_usart, crc & 0xFF);
  cocobot_com_can_send(_com_id, crc & 0xFF);
  mcual_usart_send(_usart, (crc >> 8) & 0xFF);
  cocobot_com_can_send(_com_id,(crc >> 8) & 0xFF);
  
  cocobot_com_can_flush(_com_id);

  xSemaphoreGive(_mutex);
}

void cocobot_com_printf(char * fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(_printf_buffer, sizeof(_printf_buffer), fmt, ap);
  va_end(ap);
#ifdef AUSBEE_SIM
  va_start(ap, fmt);  
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\r\n");
  va_end(ap);
#endif
  cocobot_com_send(COCOBOT_COM_PRINTF_PID, "S", _printf_buffer);
}

uint32_t cocobot_com_read_B(uint8_t *data , uint32_t len, uint32_t offset, uint8_t * value)
{
  (void)len; //TODO: check len
  *value = data[offset];
  offset += 1;
  return offset;
}

uint32_t cocobot_com_read_F(uint8_t *data , uint32_t len, uint32_t offset, float * value)
{
  (void)len; //TODO: check len
  uint8_t * ptr = (uint8_t *) value;
  
  int i;
  for(i = 0; i < 4; i += 1)
  {
    *ptr = data[offset];
    ptr += 1;
    offset += 1;
  }
  return offset;
}

uint32_t cocobot_com_read_D(uint8_t *data , uint32_t len, uint32_t offset, int32_t * value)
{
  (void)len; //TODO: check len
  uint8_t * ptr = (uint8_t *) value;
  
  int i;
  for(i = 0; i < 4; i += 1)
  {
    *ptr = data[offset];
    ptr += 1;
    offset += 1;
  }
  return offset;
}

uint32_t cocobot_com_read_H(uint8_t *data , uint32_t len, uint32_t offset, int16_t * value)
{
  (void)len; //TODO: check len
  uint32_t nv = 0;
  uint8_t * ptr = (uint8_t *) &nv;
  
  int i;
  for(i = 0; i < 2; i += 1)
  {
    *ptr = data[offset];
    ptr += 1;
    offset += 1;
  }

  *value = nv;
  return offset;
}

