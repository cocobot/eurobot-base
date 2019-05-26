#include <stdlib.h>
#include "platform.h"
#include "pcm9685.h"

#define PCM9685_ADDR 0x4F

#define PCA9685_MODE1_REGISTER 0x00
#define PCA9685_MODE1_REGISTER_SLEEP_bp 4
#define PCA9685_LED0_ON_L_REGISTER 0x06
#define PCA9685_PRE_SCALE_REGISTER 0xFE

static int is_initialized = 0;

static void pcm9685_set_sleep(uint8_t addr, uint8_t sleep)
{
  uint8_t txbuf[2];
  txbuf[0] = PCA9685_MODE1_REGISTER;
  txbuf[1] = (sleep << PCA9685_MODE1_REGISTER_SLEEP_bp) | (1 << 5);

  platform_i2c_transmit(PLATFORM_I2C_SERVO, addr, txbuf, 2, NULL, 0);
}

void pcm9685_set_frequency(uint8_t addr, uint32_t freq_Hz)
{
  uint32_t divider = 25000000 / 4096;
  divider /= freq_Hz;
  divider -= 1;

  uint8_t txbuf[2];
  txbuf[0] = PCA9685_PRE_SCALE_REGISTER;
  txbuf[1] = divider;

  platform_i2c_transmit(PLATFORM_I2C_SERVO, addr, txbuf, 2, NULL, 0);
}

static void pcm9685_init(void)
{
  pcm9685_set_sleep(PCM9685_ADDR, 1);
  pcm9685_set_frequency(PCM9685_ADDR, 50);
  pcm9685_set_sleep(PCM9685_ADDR, 0);

  is_initialized = 1;
}

void pcm9685_set_channel(uint8_t id, uint16_t delay, uint16_t width)
{
  if(!is_initialized)
  {
    pcm9685_init();
  }

  uint8_t txbuf[5];
  txbuf[0] = PCA9685_LED0_ON_L_REGISTER + 4 * (id % 16);
  txbuf[1] = delay & 0xff;
  txbuf[2] = (delay >> 8) & 0xff;
  txbuf[3] = width & 0xff;
  txbuf[4] = (width >> 8) & 0xff;

  switch(id / 16)
  {
    case 0:
      {
        platform_i2c_transmit(PLATFORM_I2C_SERVO, PCM9685_ADDR, txbuf, 5, NULL, 0);
      }
      break;
  }
}
