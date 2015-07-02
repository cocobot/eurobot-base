#ifndef MCUAL_ARCH_H
#define MCUAL_ARCH_H

#include <stdint.h>

#ifdef __AVR__

void mcual_arch_avr_ccpwrite(volatile uint8_t *address, uint8_t value);

#endif //__AVR__

#endif //MCUAL_ARCH_H
