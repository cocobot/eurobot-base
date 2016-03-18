#ifndef MCUAL_I2C_H
#define MCUAL_I2C_H

#include <stdint.h>

typedef uint32_t mcual_i2c_id_t;

#define MCUAL_I2C1  ((mcual_i2c_id_t) 1)
#define MCUAL_I2C2  ((mcual_i2c_id_t) 2)
#define MCUAL_I2C3  ((mcual_i2c_id_t) 3)
#define MCUAL_I2C4  ((mcual_i2c_id_t) 4)

typedef enum
{
  MCUAL_I2C_SUCCESS,
  MCUAL_I2C_FAIL,
} mcual_i2c_status_t;

void mcual_i2c_master_init(mcual_i2c_id_t i2c_id, uint32_t frequency_Hz);
mcual_i2c_status_t mcual_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size);

#endif


