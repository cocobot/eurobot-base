#include <mcual.h>
#include <include/generated/autoconf.h>
#ifdef CONFIG_DEVICE_STM32L496xx
# include <stm32l4xx.h>
#else
# include <stm32f4xx.h>
#endif


void mcual_bootloader(void)
{
  NVIC_SystemReset();
}

void mcual_get_unique_id(uint8_t buffer[12])
{
#ifdef CONFIG_DEVICE_STM32L496xx
  uint8_t * ptr = (uint8_t *)&buffer[0];
#else
  uint8_t * ptr = (uint8_t *)0x1FFF7A10;
#endif
  int i;
  for(i = 0; i < 12; i += 1, ptr += 1)
  {
    buffer[i] = *ptr;
  }
}

void vApplicationTickHook(void)
{
}
