#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <string.h>
#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

#define MCUAL_LOADER_SYNC_COMMAND "SYNC"
#define MCUAL_LOADER_CRC_COMMAND "CRC"
#define MCUAL_LOADER_UNKNOWN_COMMAND "UNKNOWN COMMAND"
#define MCUAL_LOADER_OK "OK"
#define MCUAL_LOADER_LINE_END '\n'
#define MCUAL_LOADER_SEPARATOR ' '


static mcual_usart_id_t _usart;
static void (*_event)(void);

static void mcual_loader_send(char c)
{
  mcual_usart_send(_usart, c);
}

static void mcual_loader_send_string(char * str)
{
  while(*str != 0)
  {
    mcual_loader_send(*str);
    str += 1;
  }
}

static void mcual_loader_send_hex_uint32(uint32_t u)
{
  mcual_loader_send('0');
  mcual_loader_send('x');
  
  int i;
  for(i = 0; i < 8; i += 1)
  {
    uint32_t b4 = u & (0x0F << ((7 - i) * 4));
    b4 >>= ((7 - i) * 4);
    if(b4 < 10)
    {
      mcual_loader_send(b4 + '0');
    }
    else
    {
      mcual_loader_send(b4 + 'A' - 10);
    }
  }
}

static void mcual_loader_blink(void)
{
  int i;
  for(i = 0; i < 10; i += 1)
  {
    _event();
    volatile uint32_t j;
    for(j = 0; j < 300000; j += 1);
  }
}

static uint32_t mcual_loader_compute_crc(uint32_t pid)
{
  CRC->CR = CRC_CR_RESET;

  uint32_t * ptr = (uint32_t *)(pid * 16 * 1024);
  int i;
  for(i = 0; i < 16 * 1024; i += 4, ptr += 1)
  {
    CRC->DR = *ptr;
  }

  return CRC->DR;
}

void mcual_loader_init(mcual_usart_id_t usart_id, void (*event)(void))
{
  //internal storage initialization
  _usart = usart_id;
  _event = event;

  //loader module initialization
  mcual_usart_init(usart_id, 115200);
  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
}

void mcual_loader_run(void)
{
  char data[128];
  uint32_t position = 0;
  uint32_t no_data_cnt = 0;

  mcual_loader_blink();

  while(1)
  {
    int16_t recv = mcual_usart_recv_no_wait(MCUAL_USART1);
    if(recv == -1)
    {
      no_data_cnt += 1;
    }
    else 
    {
      no_data_cnt = 0;
      if(recv == MCUAL_LOADER_LINE_END)
      {
        data[position] = 0;

        char * cmd = data;
        char * arg = "";
        uint32_t i;
        for(i = 0; i < position; i += 1)
        {
          if(data[i] == MCUAL_LOADER_SEPARATOR)
          {
            data[i] = 0;
            arg = data + i + 1;
            break;
          }
        }

        if(strcmp(cmd, MCUAL_LOADER_SYNC_COMMAND) == 0)
        {
          mcual_loader_send_string(MCUAL_LOADER_SYNC_COMMAND);
          mcual_loader_send(MCUAL_LOADER_SEPARATOR);
          mcual_loader_send_string(arg);
          mcual_loader_send(MCUAL_LOADER_SEPARATOR);
          mcual_loader_send_string(MCUAL_LOADER_OK);
        }
        else if(strcmp(cmd, MCUAL_LOADER_CRC_COMMAND) == 0)
        {
          uint32_t pid = strtoul(arg, NULL, 10);
          uint32_t crc = mcual_loader_compute_crc(pid);

          mcual_loader_send_string(MCUAL_LOADER_CRC_COMMAND);
          mcual_loader_send(MCUAL_LOADER_SEPARATOR);
          mcual_loader_send_hex_uint32(crc);
        }
        else
        {
          mcual_loader_send_string(MCUAL_LOADER_UNKNOWN_COMMAND);
        }
        mcual_loader_send(MCUAL_LOADER_LINE_END);
        position = 0;
      }
      else
      {
        if(position < sizeof(data) - 1)
        {
          data[position] = recv;
          position += 1;
        }
      }
    }

    if(no_data_cnt > 10000000)
    {
      no_data_cnt = 0;
      mcual_loader_blink();
    }
  }
}

#endif
