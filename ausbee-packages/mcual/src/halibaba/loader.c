#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <stm32l4xx.h>

__attribute__ ((__section__(".data")))
void mcual_loader_flash_byte(uint32_t offset, uint8_t data)
{
  __disable_irq();
  FLASH->CR |= FLASH_CR_LOCK;
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;

  //prepare flash
  FLASH->CR = FLASH_CR_PG;
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
