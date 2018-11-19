#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <stdio.h>

void mcual_loader_boot(void)
{
  fprintf(stderr, "STUB: mcual_loader_boot()\n");
}

void mcual_loader_erase_pgm(void)
{
  fprintf(stderr, "STUB: mcual_loader_erase_pgm()\n");
}

void mcual_loader_flash_pgm(uint32_t offset, uint8_t * data, uint32_t size)
{
  fprintf(stderr, "STUB: mcual_loader_erase_pgm(offset=0x%X, data=%p, size=0x%X)\n", offset, data, size);
}

#endif

