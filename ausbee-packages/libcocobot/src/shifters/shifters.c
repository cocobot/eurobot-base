#include <cocobot.h>
#include <FreeRTOS.h>
#include <platform.h>
#include <task.h>

static uint8_t _out_shifters[COCOBOT_SHIFTERS_OUT_NUMBER];


void cocobot_shifters_init(void)
{
  unsigned int i;

  platform_gpio_set_direction(PLATFORM_GPIO_SHIFT_SET, MCUAL_GPIO_OUTPUT);
  platform_gpio_clear(PLATFORM_GPIO_SHIFT_SET);
  
  for(i = 0; i < sizeof(_out_shifters)/sizeof(_out_shifters[0]); i += 1)
  {
    _out_shifters[i] = 0;
  }
}

void cocobot_shifters_set(uint32_t bit, uint8_t value)
{
  if(value)
  {
    _out_shifters[bit / 8] |= (1 << (bit % 8));
  }
  else
  {
    _out_shifters[bit / 8] &= ~(1 << (bit % 8));
  }
}

void cocobot_shifters_update(void)
{
  unsigned int i;
  platform_spi_slave_select(PLATFORM_SPI_SHIFT_SELECT);

  for(i = 0; i < sizeof(_out_shifters)/sizeof(_out_shifters[0]); i += 1)
  {
    platform_spi_slave_transfert(_out_shifters[i]);
  }
  
  platform_gpio_set(PLATFORM_GPIO_SHIFT_SET);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  platform_gpio_clear(PLATFORM_GPIO_SHIFT_SET);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  platform_gpio_set(PLATFORM_GPIO_SHIFT_SET);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  platform_gpio_clear(PLATFORM_GPIO_SHIFT_SET);
  platform_spi_slave_select(PLATFORM_SPI_CS_UNSELECT);
}
