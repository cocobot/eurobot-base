#include <include/generated/autoconf.h>

#include <stdlib.h>
#include <mcual.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcual_arch_avr_ccpwrite(volatile uint8_t *address, uint8_t value)
{
  cli();
  volatile uint8_t * tmpAddr = address;
#ifdef RAMPZ
  RAMPZ = 0;
#endif
  asm volatile(
      "movw r30,  %0"        "\n\t"
      "ldi  r16,  %2"        "\n\t"
      "out   %3, r16"        "\n\t"
      "st     Z,  %1"       "\n\t"
      :
      : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
      : "r16", "r30", "r31"
     );
  sei();
}
