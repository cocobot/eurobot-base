#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <platform.h>
#ifdef CONFIG_DEVICE_STM32L496xx
# include <stm32l4xx.h>
#else
# include <stm32f4xx.h>
#endif



void mcual_loader_boot(void)
{
  uint32_t *reset = (uint32_t *)(PLATFORM_FLASH_PGM_START);

  if(*reset != 0xFFFFFFFF)
  {
    FLASH->CR = FLASH_CR_LOCK;
    USART1->CR1 = 0;
    SCB->VTOR = PLATFORM_FLASH_PGM_START;
    __set_MSP(*((uint32_t*)PLATFORM_FLASH_SECTOR_PGM_START));
    void (*boot)(void) __attribute__((noreturn)) = (void *)(*(uint32_t*)(PLATFORM_FLASH_SECTOR_PGM_START + 4));
    boot();
  }
}

void mcual_loader_erase_pgm(void)
{
  int i;

  //unlock flash
  __disable_irq();
  FLASH->CR |= FLASH_CR_LOCK;
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;

  //erase pgm flash. This will temporally lock the cpu (no com !)
  while(FLASH->SR & FLASH_SR_BSY);
  for(i = PLATFORM_FLASH_SECTOR_PGM_START; i < PLATFORM_FLASH_SECTOR_PGM_END; i += 1)
  {
#ifdef FLASH_CR_PSIZE_1
    FLASH->CR = (i << 3) | FLASH_CR_SER | FLASH_CR_PSIZE_1;                    
#else
    FLASH->CR = (i << 3);
#endif
    FLASH->CR |= FLASH_CR_STRT;
    while(FLASH->SR & FLASH_SR_BSY);
  }
  FLASH->CR = 0;
  __enable_irq();
}


void mcual_loader_flash_pgm(uint32_t offset, uint8_t * data, uint32_t size)
{
  unsigned int i;

  //prepare flash
#ifdef FLASH_CR_PSIZE_1
  FLASH->CR = FLASH_CR_PG | FLASH_CR_PSIZE_1;
#else
  FLASH->CR = FLASH_CR_PG;
#endif
  while(FLASH->SR & FLASH_SR_BSY);

  //write data
  uint32_t * ptr = (uint32_t *)(PLATFORM_FLASH_PGM_START + offset);
  for(i = 0; i < size; i += 4, ptr += 1)
  {
    uint32_t value;

    value = data[i + 0];
    value |= (data[i + 1] << 8);
    value |= (data[i + 2] << 16);
    value |= (data[i + 3] << 24);

    *ptr = value;
    while(FLASH->SR & FLASH_SR_BSY);
  }

  //clean up register
  FLASH->CR = 0;
}

__attribute__ ((__section__(".data")))
void mcual_loader_flash_byte(uint32_t offset, uint8_t data)
{
  __disable_irq();
  FLASH->CR |= FLASH_CR_LOCK;
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;

  //prepare flash
#ifdef FLASH_CR_PSIZE_1
  FLASH->CR = FLASH_CR_PG | FLASH_CR_PSIZE_1;
#else
  FLASH->CR = FLASH_CR_PG;
#endif
  while(FLASH->SR & FLASH_SR_BSY);

  //write data
  uint32_t * ptr = (uint32_t *)offset;
  *ptr = data;
  while(FLASH->SR & FLASH_SR_BSY);

  //clean up register
  FLASH->CR = 0;
  __enable_irq();
}

#endif
