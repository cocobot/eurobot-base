#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <platform.h>
#if CONFIG_DEVICE_STM32L496xx
# include <stm32l4xx.h>
#else
# include <stm32f4xx.h>
#endif



void mcual_loader_boot(void)
{
  uint32_t *reset = (uint32_t *)(PLATFORM_FLASH_PGM_START);

  if(*reset != 0xFFFFFFFF)
  {
    __disable_irq();

    unsigned int i;
    for(i = 0; i < sizeof(NVIC->ICER)/sizeof(NVIC->ICER[0]); i += 1)
    {
      NVIC->ICER[i] = 0xFFFFFFFF;
    }

    for(i = 0; i < sizeof(NVIC->ICPR)/sizeof(NVIC->ICPR[0]); i += 1)
    {
      NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    FLASH->CR = FLASH_CR_LOCK;
    USART1->CR1 = 0;
    SCB->VTOR = PLATFORM_FLASH_PGM_START;
    __set_MSP(*(uint32_t*)(PLATFORM_FLASH_PGM_START));
    void (*boot)(void) __attribute__((noreturn)) = (void *)(*(uint32_t*)(PLATFORM_FLASH_PGM_START + 4));
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
  for(i = PLATFORM_FLASH_SECTOR_PGM_START; i <= PLATFORM_FLASH_SECTOR_PGM_END; i += 1)
  {
#ifdef FLASH_CR_PSIZE_1
    FLASH->CR = (i << 3) | FLASH_CR_SER | FLASH_CR_PSIZE_1;                    
#else
    FLASH->SR = 0xFFFFFFFF;
    FLASH->CR = (i << 3) | FLASH_CR_PER;
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
  FLASH->ACR &= ~FLASH_ACR_DCEN;

  //prepare flash
#ifdef FLASH_CR_PSIZE_1
  FLASH->CR = FLASH_CR_PG | FLASH_CR_PSIZE_1;
  while(FLASH->SR & FLASH_SR_BSY);
#else
  while(FLASH->SR & FLASH_SR_BSY);
  FLASH->SR = 0xFFFFFFFF;
  FLASH->CR = FLASH_CR_PG;
#endif

  //write data
  uint32_t * ptr = (uint32_t *)(PLATFORM_FLASH_PGM_START + offset);
#if CONFIG_DEVICE_STM32L496xx
  for(i = 0; i < size; i += 8, ptr += 2)
#else
  for(i = 0; i < size; i += 4, ptr += 1)
#endif
  {
    uint32_t value;

#if CONFIG_DEVICE_STM32L496xx
    value = data[i + 0];
    value |= (data[i + 1] << 8);
    value |= (data[i + 2] << 16);
    value |= (data[i + 3] << 24);
    *ptr = value;
#else
    value = data[i + 4];
    value |= (data[i + 5] << 8);
    value |= (data[i + 6] << 16);
    value |= (data[i + 7] << 24);
    *(ptr + 1) = value;
#endif
    while(FLASH->SR & FLASH_SR_BSY);
#if CONFIG_DEVICE_STM32L496xx
    FLASH->SR = 0xFFFFFFFF;
#endif
  }

  //clean up register
  FLASH->CR = 0;
}

__attribute__ ((__section__(".data#")))
void mcual_loader_flash_u64(uint32_t offset, uint64_t data)
{
  __disable_irq();

  FLASH->ACR &= ~FLASH_ACR_DCEN;

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
  volatile uint64_t * ptr = (volatile uint64_t *)offset;
  *ptr = data;
  while(FLASH->SR & FLASH_SR_BSY);

  //clean up register
  FLASH->CR = 0;
  __enable_irq();
}

#endif
