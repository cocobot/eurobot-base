#include <mcual.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define MCUAL_USART_NUMBER (MCUAL_USART8 + 1)

QueueHandle_t tx_queues[MCUAL_USART_NUMBER];
QueueHandle_t rx_queues[MCUAL_USART_NUMBER];

static int mcual_usart_get_tx_buffer_size(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_0_TX_SIZE
    case MCUAL_USART0:
      return CONFIG_MCUAL_USART_0_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_1_TX_SIZE
    case MCUAL_USART1:
      return CONFIG_MCUAL_USART_1_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_2_TX_SIZE
    case MCUAL_USART2:
      return CONFIG_MCUAL_USART_2_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_2_TX_SIZE
    case MCUAL_USART2:
      return CONFIG_MCUAL_USART_2_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_3_TX_SIZE
    case MCUAL_USART3:
      return CONFIG_MCUAL_USART_3_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_4_TX_SIZE
    case MCUAL_USART4:
      return CONFIG_MCUAL_USART_4_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_5_TX_SIZE
    case MCUAL_USART5:
      return CONFIG_MCUAL_USART_5_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_6_TX_SIZE
    case MCUAL_USART6:
      return CONFIG_MCUAL_USART_6_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_7_TX_SIZE
    case MCUAL_USART7:
      return CONFIG_MCUAL_USART_7_TX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_8_TX_SIZE
    case MCUAL_USART8:
      return CONFIG_MCUAL_USART_8_TX_SIZE;
#endif
  }

  return 0;
}

static int mcual_usart_get_rx_buffer_size(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_0_RX_SIZE
    case MCUAL_USART0:
      return CONFIG_MCUAL_USART_0_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_1_RX_SIZE
    case MCUAL_USART1:
      return CONFIG_MCUAL_USART_1_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_2_RX_SIZE
    case MCUAL_USART2:
      return CONFIG_MCUAL_USART_2_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_2_RX_SIZE
    case MCUAL_USART2:
      return CONFIG_MCUAL_USART_2_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_3_RX_SIZE
    case MCUAL_USART3:
      return CONFIG_MCUAL_USART_3_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_4_RX_SIZE
    case MCUAL_USART4:
      return CONFIG_MCUAL_USART_4_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_5_RX_SIZE
    case MCUAL_USART5:
      return CONFIG_MCUAL_USART_5_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_6_RX_SIZE
    case MCUAL_USART6:
      return CONFIG_MCUAL_USART_6_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_7_RX_SIZE
    case MCUAL_USART7:
      return CONFIG_MCUAL_USART_7_RX_SIZE;
#endif

#ifdef CONFIG_MCUAL_USART_8_RX_SIZE
    case MCUAL_USART8:
      return CONFIG_MCUAL_USART_8_RX_SIZE;
#endif
  }

  return 0;
}

void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate)
{
  (void)baudrate;

  tx_queues[usart_id] = xQueueCreate(4 * mcual_usart_get_tx_buffer_size(usart_id), sizeof(uint8_t));
  rx_queues[usart_id] = xQueueCreate(4 * mcual_usart_get_rx_buffer_size(usart_id), sizeof(uint8_t));
}



void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte)
{
  mcual_arch_sim_handle_uart_peripheral_write(usart_id, byte);
}

void mcual_usart_recv_from_network(mcual_usart_id_t usart_id, uint8_t byte)
{
  xQueueSendFromISR(rx_queues[usart_id], &byte, NULL);
}

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id)
{
  uint8_t byte;
  xQueueReceive(rx_queues[usart_id], &byte, portMAX_DELAY);
  return byte;
}

int16_t mcual_usart_recv_no_wait(mcual_usart_id_t usart_id)
{
  uint8_t byte;
  if(xQueueReceive(rx_queues[usart_id], &byte, 0) == pdFALSE)
  {
    return -1;
  }
  return byte;

}

