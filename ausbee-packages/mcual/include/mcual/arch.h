#ifndef MCUAL_ARCH_H
#define MCUAL_ARCH_H

#include <stdint.h>

#ifdef __AVR__

void mcual_arch_avr_ccpwrite(volatile uint8_t *address, uint8_t value);

#endif //__AVR__

#ifdef AUSBEE_SIM
void mcual_arch_sim_init_peripherals(void);
void mcual_arch_sim_handle_uart_peripheral_write(mcual_usart_id_t usart_id, uint8_t byte);
void mcual_usart_recv_from_network(mcual_usart_id_t usart_id, uint8_t byte);
void mcual_arch_main(int argc, char *argv[]); 
#endif //AUSBEE_SIM

void mcual_bootloader(void);
void mcual_get_unique_id(uint8_t buffer[12]);


#endif //MCUAL_ARCH_H
