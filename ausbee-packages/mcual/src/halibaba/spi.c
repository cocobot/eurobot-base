#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_SPI

#pragma GCC mcual_spi_master_init mcual_spi_master_transfert

void mcual_spi_master_init(mcual_spi_id_t spi_id, mcual_spi_mode_t spi_mode, uint32_t frequency_Hz)
{
  (void)spi_id;
  (void)spi_mode;
  (void)frequency_Hz;
}

uint8_t mcual_spi_master_transfert(mcual_spi_id_t spi_id, uint8_t byte)
{
  (void)byte;
  return 0xFF;
}
#endif
