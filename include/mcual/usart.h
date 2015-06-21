#ifndef MCUAL_USART_H
#define MCUAL_USART_H

#include <stdint.h>

typedef uint32_t mcual_usart_id_t;

#define MCUAL_USART0  ((mcual_usart_id_t) 0)
#define MCUAL_USART1  ((mcual_usart_id_t) 1)
#define MCUAL_USART2  ((mcual_usart_id_t) 2)
#define MCUAL_USART3  ((mcual_usart_id_t) 3)
#define MCUAL_USART4  ((mcual_usart_id_t) 4)
#define MCUAL_USART5  ((mcual_usart_id_t) 5)
#define MCUAL_USART6  ((mcual_usart_id_t) 6)
#define MCUAL_USART7  ((mcual_usart_id_t) 7)
#define MCUAL_USART8  ((mcual_usart_id_t) 8)

void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate);
void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte);

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id);
int16_t mcual_usart_recv_no_wait(mcual_usart_id_t usart_id);

#endif
