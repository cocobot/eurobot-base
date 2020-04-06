#ifndef MCUAL_LOADER_H
#define MCUAL_LOADER_H

void mcual_loader_boot(void);
void mcual_loader_erase_pgm(void);
void mcual_loader_flash_pgm(uint32_t offset, uint8_t * data, uint32_t size);
void mcual_loader_flash_u64(uint32_t offset, uint64_t data);
uint32_t mcual_loader_compute_crc_16k(uint32_t addr);

#endif // MCUAL_LOADER_H
