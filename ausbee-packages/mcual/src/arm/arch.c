#include <mcual.h>
#include <stm32f4xx.h>

void mcual_bootloader(void)
{
  NVIC_SystemReset();
}

void mcual_get_unique_id(uint8_t buffer[12])
{
  int i;
  uint8_t * ptr = (uint8_t *)0x1FFF7A10;
  for(i = 0; i < 12; i += 1, ptr += 1)
  {
    buffer[i] = *ptr;
  }
}

void vApplicationTickHook(void)
{
}
