#include <mcual.h>


void mcual_i2c_master_init(mcual_i2c_id_t i2c_id, uint32_t frequency_Hz)
{
  (void)i2c_id;
  (void)frequency_Hz;
}

mcual_i2c_status_t mcual_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
{
  (void)id;
  (void)addr;
  (void)txbuf;
  (void)tx_size;
  (void)rxbuf;
  (void)rx_size;
  return MCUAL_I2C_FAIL;
}
