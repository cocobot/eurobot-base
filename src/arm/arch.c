#include <mcual.h>
#include <stm32f4xx.h>

void mcual_bootloader(void)
{
  NVIC_SystemReset();
}
