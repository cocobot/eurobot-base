#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_USART

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
#include <FreeRTOS.h>
#include <queue.h>
#endif

#define MCUAL_USART_NUMBER (MCUAL_USART8 + 1)

#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
xQueueHandle tx_queues[MCUAL_USART_NUMBER];
xQueueHandle rx_queues[MCUAL_USART_NUMBER];
#endif

static USART_TypeDef * mcual_usart_get_register(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
    case MCUAL_USART1:
      return USART1;

    case MCUAL_USART2:
      return USART2;

    case MCUAL_USART3:
      return USART3;

    case MCUAL_USART4:
      return UART4;

    case MCUAL_USART5:
      return UART5;

    case MCUAL_USART6:
      return USART6;

    case MCUAL_USART7:
      return UART7;

    case MCUAL_USART8:
      return UART8;
  }

  return NULL;
}

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
  USART_TypeDef * reg = mcual_usart_get_register(usart_id);
  mcual_clock_id_t clock = MCUAL_CLOCK_PERIPHERAL_1;

  //enable clock
  switch(usart_id)
  {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case 4:
      RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case 5:
      RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case 6:
      RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
      clock = MCUAL_CLOCK_PERIPHERAL_2;
      break;

    case 7:
      RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case 8:
      RCC->APB1ENR |= RCC_APB1ENR_UART8EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;
  }

  //compute baudrate
  float pres = (float)(mcual_clock_get_frequency_Hz(clock)) / (baudrate * 16.0);
  int pres_i = (pres * 16.0);

  //set baudrate
  reg->BRR = pres_i;

  //reset SPI parameters and hardware flow control
  reg->CR2 = 0;
  reg->CR3 = 0;
  
  //enable usart tx and rx
  reg->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  tx_queues[usart_id] = xQueueCreate(mcual_usart_get_tx_buffer_size(usart_id), sizeof(uint8_t));
  rx_queues[usart_id] = xQueueCreate(mcual_usart_get_rx_buffer_size(usart_id), sizeof(uint8_t));
#else
#error not implemented yet
#endif
}

void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  xQueueSend(tx_queues[usart_id], &byte, portMAX_DELAY);
#else
  (void)usart_id;
  (void)byte;
#error not implemented yet
#endif
}

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id)
{
  uint8_t byte;
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  xQueueReceive(rx_queues[usart_id], &byte, portMAX_DELAY);
#else
  (void)usart_id;
  byte = 0;
#endif
  return byte;
}

int16_t mcual_usart_recv_no_wait(mcual_usart_id_t usart_id)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  uint8_t byte;
  if(xQueueReceive(rx_queues[usart_id], &byte, 0) == pdFALSE)
  {
    return -1;
  }
  return byte;
#else
  (void)usart_id;
#endif
  return -1;
}

#endif
