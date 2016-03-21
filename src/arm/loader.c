#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <string.h>
#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

#define MCUAL_LOADER_SYNC_COMMAND "SYNC"
#define MCUAL_LOADER_CRC_COMMAND "CRC"
#define MCUAL_LOADER_ERASE_COMMAND "ERASE"
#define MCUAL_LOADER_FLASH_COMMAND "FLASH"
#define MCUAL_LOADER_BOOT_COMMAND "BOOT"
#define MCUAL_LOADER_UNKNOWN_COMMAND "UNKNOWN COMMAND"
#define MCUAL_LOADER_OK "OK"
#define MCUAL_LOADER_KO "KO"
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

void HardFault_Handler(void)
{
  while(1)
  {
    int k;
    for(k = 0; k < 5; k += 1)
    {
      _event();
      volatile uint32_t j;
      for(j = 0; j < 3000000; j += 1);
    }
  }
}

void mcual_loader_boot(void)
{
  uint32_t *reset = (uint32_t *)(0x08004004);

  if(*reset != 0xFFFFFFFF)
  {
    FLASH->CR = FLASH_CR_LOCK;
    USART1->CR1 = 0;
    SCB->VTOR = 0x08004000;
    __set_MSP(*((uint32_t*)0x08004000));
    void (*boot)(void) __attribute__((noreturn)) = (void *)(*(uint32_t*)(0x08004004));
    boot();
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

  FLASH->CR |= FLASH_CR_LOCK;
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}

void mcual_loader_run(void)
{
  char data[128];
  uint32_t position = 0;
  uint32_t no_data_cnt = 0;
  int autoboot = 1;

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
          autoboot = 0;
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
        else if(strcmp(cmd, MCUAL_LOADER_ERASE_COMMAND) == 0)
        {
          for(i = 1; i < 12; i += 1)
          {
            while(FLASH->SR & FLASH_SR_BSY);
            FLASH->CR = (i << 3) | FLASH_CR_SER | FLASH_CR_PSIZE_1;                    
            FLASH->CR |= FLASH_CR_STRT;
            while(FLASH->SR & FLASH_SR_BSY);
          }
          FLASH->CR = 0;

          mcual_loader_send_string(MCUAL_LOADER_ERASE_COMMAND);
          mcual_loader_send(MCUAL_LOADER_SEPARATOR);
          mcual_loader_send_string(MCUAL_LOADER_OK);
        }
        else if(strcmp(cmd, MCUAL_LOADER_FLASH_COMMAND) == 0)
        {
          uint32_t pid = strtoul(arg, NULL, 10);
          if(pid > 8192)
          {
            uint32_t * ptr = (uint32_t *)(pid * 16 * 1024);

            FLASH->CR = FLASH_CR_PG | FLASH_CR_PSIZE_1;
            while(FLASH->SR & FLASH_SR_BSY);
            for(i = 0; i < 16 * 1024; i += 4, ptr += 1)
            {
              uint32_t value;

              value = mcual_usart_recv(MCUAL_USART1);
              value |= (mcual_usart_recv(MCUAL_USART1) << 8);
              value |= (mcual_usart_recv(MCUAL_USART1) << 16);
              value |= (mcual_usart_recv(MCUAL_USART1) << 24);

              *ptr = value;
              while(FLASH->SR & FLASH_SR_BSY);
            }
            FLASH->CR = 0;

            mcual_loader_send_string(MCUAL_LOADER_FLASH_COMMAND);
            mcual_loader_send(MCUAL_LOADER_SEPARATOR);
            mcual_loader_send_string(MCUAL_LOADER_OK);
          }
          else
          {
            mcual_loader_send_string(MCUAL_LOADER_FLASH_COMMAND);
            mcual_loader_send(MCUAL_LOADER_SEPARATOR);
            mcual_loader_send_string(MCUAL_LOADER_KO);
          }
        }
        else if(strcmp(cmd, MCUAL_LOADER_BOOT_COMMAND) == 0)
        {
          mcual_loader_send_string(MCUAL_LOADER_BOOT_COMMAND);
          mcual_loader_send(MCUAL_LOADER_SEPARATOR);
          mcual_loader_send_string(MCUAL_LOADER_KO);
          mcual_loader_send(MCUAL_LOADER_LINE_END);

          mcual_loader_boot();
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
      if(autoboot)
      {
        mcual_loader_boot();
      }
    }
  }
}

#endif
