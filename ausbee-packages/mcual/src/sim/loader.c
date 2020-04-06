#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_LOADER

#include <mcual.h>
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <unistd.h>
#include <limits.h>

extern int _argc;
extern char ** _argv;

static uint8_t fake_flash[1 * 1024 * 1024];


//loaders
//boards
void mcual_loader_boot(void)
{
  char cwd[PATH_MAX];
  char loader_path[PATH_MAX];
  char argv[PATH_MAX];
  char * prj;
  char * tmp;

  //get current directory
  getcwd(cwd, sizeof(cwd));

  //extract prj name from exe path
  memcpy(argv, _argv[0], sizeof(argv));
  tmp = argv;
  while((tmp = strtok(tmp, "/")) != NULL)
  {
    prj = tmp;
    tmp = NULL;
  }
  prj = strtok(prj, ".");

  //create new path
  snprintf(loader_path, sizeof(loader_path), "%s/../../boards/%s/Output/%s.sim", cwd, prj, prj);

  //start executable
  fprintf(stdout, "START: %s -> %s\n", cwd, loader_path);
  system(loader_path);

  exit(0);
}

void mcual_loader_erase_pgm(void)
{
  fprintf(stdout, "STUB: mcual_loader_erase_pgm()\n");
  portENTER_CRITICAL();
  sleep(10);
  memset(fake_flash, 0xFF, sizeof(fake_flash));
  portEXIT_CRITICAL();
  fprintf(stdout, "STUB: mcual_loader_erase_pgm() done\n");
}

void mcual_loader_flash_pgm(uint32_t offset, uint8_t * data, uint32_t size)
{
  fprintf(stdout, "STUB: mcual_loader_flash_pgm(offset=0x%X, data=%p, size=0x%X)\n", offset, data, size);
  offset -= 0x8000000;
  if(offset + size < sizeof(fake_flash)) 
  {
    memcpy(&fake_flash[offset], data, size);
  }
  else
  {
    fprintf(stdout, "STUB: mcual_loader_flash_pgm: ERROR offset/size\n");
  }
}

void mcual_loader_flash_u64(uint32_t offset, uint64_t data)
{
  fprintf(stdout, "STUB: mcual_loader_flash_byte(offset=0x%X, data=0x%X)\n", offset, data);
}

/**
 * @brief Get the crc value of the 16k following bytes
 * @param addr  Address of the first byte
 * @return  The computed crc
 */
uint32_t mcual_loader_compute_crc_16k(uint32_t addr)
{
  uint32_t crc = 0xFFFFFFFF;
  const uint32_t polynom = 0x04C11DB7;

  uint32_t * ptr = (uint32_t *)(&fake_flash[addr - 0x8000000]);
  int i;
  int j;
  for(i = 0; i < 16 * 1024; i += 4, ptr += 1)
  {
    crc = crc ^ *ptr;
    for(j = 0; j < 32; j += 1)
    {
      if(crc & 0x80000000)
      {
        crc = (crc << 1) ^ polynom;
      }
      else
      {
        crc = (crc << 1);
      }
    }
  }

  fprintf(stdout, "STUB: mcual_loader_compute_crc_16k(offset=0x%X, crc=0x%X)\n", addr, crc);
  return crc;
}

#endif

