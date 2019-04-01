#ifndef MCUAL_ARCH_H
#define MCUAL_ARCH_H

#include <stdint.h>

#ifdef __AVR__

void mcual_arch_avr_ccpwrite(volatile uint8_t *address, uint8_t value);

#endif //__AVR__

#ifdef AUSBEE_SIM
void mcual_arch_init(void);
void mcual_arch_request(char * mod, int id, char * fmt, ...);
#endif //AUSBEE_SIM

void mcual_bootloader(void);
void mcual_get_unique_id(uint8_t buffer[12]);


#endif //MCUAL_ARCH_H
