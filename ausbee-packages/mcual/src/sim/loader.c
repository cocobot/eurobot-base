#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>


void mcual_loader_boot(void)
{
  mcual_arch_request("ARCH", 0, "REBOOT");
  exit(0);
}

void mcual_loader_erase_pgm(void)
{
  fprintf(stdout, "STUB: mcual_loader_erase_pgm()\n");
  portENTER_CRITICAL();
  sleep(10);
  portEXIT_CRITICAL();
}

void mcual_loader_flash_pgm(uint32_t offset, uint8_t * data, uint32_t size)
{
  fprintf(stdout, "STUB: mcual_loader_flash_pgm(offset=0x%X, data=%p, size=0x%X)\n", offset, data, size);
}

void mcual_loader_flash_byte(uint32_t offset, uint8_t data)
{
  fprintf(stdout, "STUB: mcual_loader_flash_byte(offset=0x%X, data=0x%X)\n", offset, data);
}

#endif

