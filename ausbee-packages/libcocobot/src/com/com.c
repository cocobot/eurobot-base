#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define COCOBOT_COM_HEADER_START 0xC0

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

void cocobot_com_send(uint16_t pid, char * fmt, ...)
{
  va_list(ap);

  cocobot_com_header_t header;

  header.start = COCOBOT_COM_HEADER_START;
  header.pid = pid;
  header.len = 0;
  size_t i;
  for(i = 0; i < strlen(fmt); i += 1)
  {
    switch(fmt[i])
    {
      case 'H':
        header.len += 2;
        break;

      case 'F':
        header.len += 4;
        break;
    }
  }
  header.crc = 0xFFFF;
  uint8_t * ptr = (uint8_t *)&header;
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
  va_start(ap, fmt);
  for(i = 0; i < strlen(fmt); i += 1)
  {
    switch(fmt[i])
    {
      case 'H':
        {
          uint16_t v = va_arg(ap, unsigned int) & 0xFFFF;
          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
        }
        break;

      case 'F':
        {
          float v = va_arg(ap, double);
          uint8_t * p = (uint8_t *)&v;
          mcual_usart_send(_usart, *(p + 0));
          mcual_usart_send(_usart, *(p + 1));
          mcual_usart_send(_usart, *(p + 2));
          mcual_usart_send(_usart, *(p + 3));
        }
        break;
    }
  }
  va_end(ap);

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
