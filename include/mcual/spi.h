#ifndef MCUAL_SPI_H
#define MCUAL_SPI_H

#include <stdint.h>

typedef uint32_t mcual_spi_id_t;
typedef enum
{
  MCUAL_SPI_MODE_0,
  MCUAL_SPI_MODE_1,
  MCUAL_SPI_MODE_2,
  MCUAL_SPI_MODE_3,
} mcual_spi_mode_t;

#define MCUAL_SPI0  ((mcual_spi_id_t) 0)
#define MCUAL_SPI1  ((mcual_spi_id_t) 1)
#define MCUAL_SPI2  ((mcual_spi_id_t) 2)
#define MCUAL_SPI3  ((mcual_spi_id_t) 3)
#define MCUAL_SPI4  ((mcual_spi_id_t) 4)
#define MCUAL_SPI5  ((mcual_spi_id_t) 5)
#define MCUAL_SPI6  ((mcual_spi_id_t) 6)


void mcual_spi_master_init(mcual_spi_id_t spi_id, mcual_spi_mode_t spi_mode, uint32_t frequency_Hz);
uint8_t mcual_spi_master_transfert(mcual_spi_id_t spi_id, uint8_t byte);


#endif

