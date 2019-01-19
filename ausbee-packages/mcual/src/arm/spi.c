#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_SPI

#include <mcual.h>
#include <stm32f4xx.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

static SPI_TypeDef * mcual_spi_get_register(mcual_spi_id_t spi_id)
{
  switch(spi_id)
  {
    case MCUAL_SPI1:
      return SPI1;

    case MCUAL_SPI2:
      return SPI2;

    case MCUAL_SPI3:
      return SPI3;

    case MCUAL_SPI4:
      return SPI4;

    case MCUAL_SPI5:
      return SPI5;

    case MCUAL_SPI6:
      return SPI6;

  }
  return NULL;
}

void mcual_spi_master_init(mcual_spi_id_t spi_id, mcual_spi_mode_t spi_mode, uint32_t frequency_Hz)
{
  SPI_TypeDef * reg = mcual_spi_get_register(spi_id);
  mcual_clock_id_t clock = MCUAL_CLOCK_PERIPHERAL_1;

  switch(spi_id)
  {
    case MCUAL_SPI1:
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;

    case MCUAL_SPI2:
      RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case MCUAL_SPI3:
      RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case MCUAL_SPI4:
      RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;

    case MCUAL_SPI5:
      RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;

    case MCUAL_SPI6:
      RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;
  }

  int prescaler;
  for(prescaler = 0; prescaler < 7; prescaler += 1)
  {
    if(mcual_clock_get_frequency_Hz(clock) / (1 << (prescaler + 1)) <= frequency_Hz)
    {
      break;
    }
  }

  int mode = 0;
  switch(spi_mode)
  {
    case MCUAL_SPI_MODE_0:
      mode = 0;
      break;

    case MCUAL_SPI_MODE_1:
      mode = SPI_CR1_CPHA;
      break;

    case MCUAL_SPI_MODE_2:
      mode = SPI_CR1_CPOL;
      break;

    case MCUAL_SPI_MODE_3:
      mode = SPI_CR1_CPOL | SPI_CR1_CPHA;
  }

  reg->CR2 = 0;
  reg->CR1 = (prescaler << 3) | mode | SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI;
}

uint8_t mcual_spi_master_transfert(mcual_spi_id_t spi_id, uint8_t byte)
{
  SPI_TypeDef * reg = mcual_spi_get_register(spi_id);

  reg->DR = byte;
  while(!(reg->SR & SPI_SR_RXNE));
  while(!(reg->SR & SPI_SR_TXE));
  while(reg->SR & SPI_SR_BSY);

  vTaskDelay(2);
  return reg->DR;
}
#endif
