#include <stdint.h>
#include <FreeRTOS.h>
#include "platform.h"
#include "vl53l0x_api.h"

int32_t VL53L0X_write_multi(uint8_t address, uint8_t reg, uint8_t *pdata, int32_t count)
{
  uint8_t * buffer = pvPortMalloc(count + 1);
  if(buffer != NULL)
  {
    return VL53L0X_ERROR_CONTROL_INTERFACE;
  }

  memcpy(buffer + 1, pdata, count);
  buffer[0] = reg;

  mcual_i2c_status_t status = platform_i2c_transmit(PLATFORM_I2C_VL53L0X, address, buffer, count + 1, NULL, 0);

  vPortFree(buffer);

  switch(status) 
  {
    case MCUAL_I2C_SUCCESS:
      return VL53L0X_ERROR_NONE;

    default:
      return VL53L0X_ERROR_CONTROL_INTERFACE;
  }
}

int32_t VL53L0X_read_multi(uint8_t address, uint8_t reg, uint8_t *pdata, int32_t count)
{
  mcual_i2c_status_t status = platform_i2c_transmit(PLATFORM_I2C_VL53L0X, address, &reg, 1, NULL, 0);
  if(status != MCUAL_I2C_SUCCESS) 
  {
    return VL53L0X_ERROR_CONTROL_INTERFACE;
  }

  status = platform_i2c_transmit(PLATFORM_I2C_VL53L0X, address, NULL, 0, pdata, count);

  switch(status) 
  {
    case MCUAL_I2C_SUCCESS:
      return VL53L0X_ERROR_NONE;

    default:
      return VL53L0X_ERROR_CONTROL_INTERFACE;
  }
}

int32_t VL53L0X_write_byte(uint8_t address, uint8_t index, uint8_t data)
{
  return VL53L0X_write_multi(address, index, &data, 1);
}

int32_t VL53L0X_write_word(uint8_t address, uint8_t index, uint16_t data)
{
  uint8_t buf[2];
  buf[1] = data & 0xFF;
  buf[0] = data >> 8;
  return VL53L0X_write_multi(address, index, buf, 2);
}

int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t data)
{
  uint8_t buf[4];
  buf[3] = (data >>  0) & 0xFF;
  buf[2] = (data >>  8) & 0xFF;
  buf[1] = (data >> 16) & 0xFF;
  buf[0] = (data >> 24) & 0xFF;
  return VL53L0X_write_multi(address, index, buf, 4);
}

int32_t VL53L0X_read_byte(uint8_t address, uint8_t index, uint8_t *data)
{
  return VL53L0X_read_multi(address, index, data, 1);
}

int32_t VL53L0X_read_word(uint8_t address, uint8_t index, uint16_t *data)
{
  uint8_t buf[2];
  int r = VL53L0X_read_multi(address, index, buf, 2);

  uint16_t tmp;
  tmp = buf[0];
  tmp <<= 8;
  tmp |= buf[1];
  *data = tmp;

  return r;
}

int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *data)
{
  uint8_t buf[4];
  int r = VL53L0X_read_multi(address, index, buf, 4);

  uint32_t tmp;
  tmp = buf[0];
  tmp <<= 8;
  tmp |= buf[1];
  tmp <<= 8;
  tmp |= buf[2];
  tmp <<= 8;
  tmp |= buf[3];

  *data = tmp;

  return r;
}
