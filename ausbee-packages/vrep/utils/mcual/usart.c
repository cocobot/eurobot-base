#include <mcual.h>
#include <FreeRTOS.h>
#include <task.h>


void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate)
{
  (void)usart_id;
  (void)baudrate;
}



void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte)
{
  (void)usart_id;
  (void)byte;
}

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id)
{
  (void)usart_id;
  vTaskSuspend(NULL);
  return 42;
}

int16_t mcual_usart_recv_no_wait(mcual_usart_id_t usart_id)
{
  (void)usart_id;
  return -1;
}
