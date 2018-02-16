#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define COCOBOT_COM_HEADER_START 0xC0
#define INVALID_PTR ((void *)0xFFFFFFFF)

typedef struct __attribute__((packed)) 
{
  uint8_t start;
  uint16_t pid;
  uint16_t len;
  uint16_t crc;
} cocobot_com_header_t;

static SemaphoreHandle_t _mutex;
static mcual_usart_id_t _usart;

void cocobot_com_async_thread(void *arg)
{
  (void)arg;

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while(pdTRUE)
  {
    //send debug information if needed
    cocobot_position_handle_async_com();
    cocobot_asserv_handle_async_com();
    cocobot_trajectory_handle_async_com();

    //wait 100ms (minus time used by previous handler)
    vTaskDelayUntil( &xLastWakeTime, 100 / portTICK_PERIOD_MS);
  }
}

void cocobot_com_init(mcual_usart_id_t usart_id, unsigned int priority_monitor, unsigned int priority_async, cocobot_com_handler_t handler)
{
  _usart = usart_id;
  //create mutex
  _mutex = xSemaphoreCreateMutex();


  //init usart peripheral
  mcual_usart_init(_usart, 115200);

  //start tasks
  (void)priority_monitor;
  xTaskCreate(cocobot_com_async_thread, "com. async", 512, NULL, priority_async, NULL );

  (void)handler;
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

static int cocobot_com_compute_len(char ** fmt, va_list ap, uint32_t nested)
{
  int size = 0; 

  for(; (**fmt) != 0; (*fmt) += 1)
  {
    switch(**fmt)
    {
      case 'F':
        {
          size += 4;
          if(nested)
          {
            va_arg(ap, size_t); //offsetof
          }
          else
          {
            va_arg(ap, double);
          }
        }
        break;

      case 'B':
        {
          size += 1;
          if(nested)
          {
            va_arg(ap, size_t); //offsetof
          }
          else
          {
            va_arg(ap, int);
          }
        }
        break;

      case ']':
        return size;
        break;

      case '[':
        {
          va_arg(ap, uint8_t *); //ptr
          va_arg(ap, size_t); //size
          va_arg(ap, size_t); //size
          unsigned int start = va_arg(ap, unsigned int); //start
          unsigned int end = va_arg(ap, unsigned int); //end
          int mult = end - start;
          (*fmt) += 1;
          size += 2;
          size += mult * cocobot_com_compute_len(fmt, ap, 1);  
        }
        break;

      default:
#ifdef AUSBEE_SIM
        fprintf(stderr, "COM: Unknown size of %c(%s)\n", **fmt, *fmt);
#endif
        break;
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
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));
          mcual_usart_send(_usart, *(p + 2));
          crc = cocobot_com_crc16_update(crc, *(p + 2));
          mcual_usart_send(_usart, *(p + 3));
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
          crc = cocobot_com_crc16_update(crc, *(p + 0));
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

          uint16_t v = end - start;
          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          crc = cocobot_com_crc16_update(crc, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          crc = cocobot_com_crc16_update(crc, *(p + 1));

          int i;
          (*fmt) += 1;
          for(i = start; i < end; i += 1)
          {
            va_list ap_cpy;
            va_copy(ap_cpy, ap);
            char * nfmt = *fmt;
            crc = cocobot_com_send_data(&nfmt, ap_cpy, crc, ptr + elmsize * i);
            va_end(ap_cpy);
          }
          
          cocobot_com_send_data(fmt, ap, crc, INVALID_PTR);
        }
        break;

      default:
#ifdef AUSBEE_SIM
        fprintf(stderr, "COM: Unknown size of %c(%s)\n", **fmt, *fmt);
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

  header.start = COCOBOT_COM_HEADER_START;
  header.pid = pid;

  va_list ap_cpy_len;
  va_copy(ap_cpy_len, ap);
  char * pfmt = fmt;
  header.len = cocobot_com_compute_len(&pfmt, ap_cpy_len, 0);
  va_end(ap_cpy_len);

  header.crc = 0xFFFF;
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

  uint16_t crc = 0xFFFF;

  pfmt = fmt;
  crc = cocobot_com_send_data(&pfmt, ap, crc, 0);
  va_end(ap);

  //for(i = 0; i < strlen(fmt); i += 1)
  //{
  //  switch(fmt[i])
  //  {
  //    case 'H':
  //      {
  //        uint16_t v = va_arg(ap, unsigned int) & 0xFFFF;
  //        uint8_t * p = (uint8_t *)&v;
  //        mcual_usart_send(_usart, *(p + 0));
  //        mcual_usart_send(_usart, *(p + 1));
  //      }
  //      break;

  //    case 'F':
  //      {
  //        float v = va_arg(ap, double);
  //        uint8_t * p = (uint8_t *)&v;
  //        mcual_usart_send(_usart, *(p + 0));
  //        mcual_usart_send(_usart, *(p + 1));
  //        mcual_usart_send(_usart, *(p + 2));
  //        mcual_usart_send(_usart, *(p + 3));
  //      }
  //      break;
  //  }
  //}

  crc = 0x4242;
  mcual_usart_send(_usart, crc & 0xFF);
  mcual_usart_send(_usart, (crc >> 8) & 0xFF);

  xSemaphoreGive(_mutex);
}

void cocobot_com_printf(char * fmt, ...)
{
#ifdef AUSBEE_SIM
  fprintf(stderr, "%s", fmt);
#endif
  (void)fmt;
}
