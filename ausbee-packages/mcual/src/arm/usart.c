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
QueueHandle_t tx_queues[MCUAL_USART_NUMBER];
QueueHandle_t rx_queues[MCUAL_USART_NUMBER];
#else
volatile uint8_t * tx_buffer[MCUAL_USART_NUMBER];
volatile uint8_t * rx_buffer[MCUAL_USART_NUMBER];
volatile int32_t tx_buffer_read[MCUAL_USART_NUMBER];
volatile int32_t tx_buffer_write[MCUAL_USART_NUMBER];
volatile int32_t rx_buffer_read[MCUAL_USART_NUMBER];
volatile int32_t rx_buffer_write[MCUAL_USART_NUMBER];
#endif


#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
#define generateIRQHandler(n, id, name)  \
void name ## _IRQHandler(void)\
{\
  uint32_t sr = name->SR;\
\
  if(sr & USART_SR_TXE)\
  {\
\
    uint8_t byte;\
    if(xQueueReceiveFromISR(tx_queues[id], &byte, NULL) == pdTRUE)\
    {\
      name->DR = byte;\
    }\
    else\
    {\
      name->CR1 &= ~USART_CR1_TXEIE;\
    }\
  }\
\
  if(sr & (USART_SR_RXNE | USART_SR_ORE)) \
  {\
    name->SR &= ~(USART_SR_RXNE | USART_SR_ORE);\
    uint8_t byte = name->DR;\
    xQueueSendFromISR(rx_queues[id], &byte, NULL);\
  }\
}
#else
#define generateIRQHandler(n, id, name)  \
uint8_t tx_buffer_##n[CONFIG_MCUAL_USART_ ## n ## _TX_SIZE];\
uint8_t rx_buffer_##n[CONFIG_MCUAL_USART_ ## n ## _RX_SIZE];\
void name ## _IRQHandler(void)\
{\
  uint32_t sr = name->SR;\
\
  if(sr & USART_SR_TXE)\
  {\
\
    if(tx_buffer_read[id] != tx_buffer_write[id])\
    {\
      name->DR = tx_buffer_##n[tx_buffer_read[id]];\
      tx_buffer_read[id] += 1;\
      if(tx_buffer_read[id] >= CONFIG_MCUAL_USART_##n##_TX_SIZE)\
      {\
        tx_buffer_read[id] = 0;\
      }\
    }\
    else\
    {\
      name->CR1 &= ~USART_CR1_TXEIE;\
    }\
  }\
\
  if(sr & (USART_SR_RXNE | USART_SR_ORE)) \
  {\
    name->SR &= ~(USART_SR_RXNE | USART_SR_ORE);\
    rx_buffer_##n[rx_buffer_write[id]] = name->DR;\
    rx_buffer_write[id] += 1;\
    if(rx_buffer_write[id] >= CONFIG_MCUAL_USART_##n##_RX_SIZE)\
    {\
      rx_buffer_write[id] = 0;\
    }\
  }\
}
#endif

#ifdef CONFIG_MCUAL_USART_1
generateIRQHandler(1, MCUAL_USART1, USART1);
#endif
#ifdef CONFIG_MCUAL_USART_2
generateIRQHandler(2, MCUAL_USART2, USART2);
#endif
#ifdef CONFIG_MCUAL_USART_3
generateIRQHandler(3, MCUAL_USART3, USART3);
#endif
#ifdef CONFIG_MCUAL_USART_4
generateIRQHandler(4, MCUAL_USART4, UART4);
#endif
#ifdef CONFIG_MCUAL_USART_5
generateIRQHandler(5, MCUAL_USART5, UART5);
#endif
#ifdef CONFIG_MCUAL_USART_6
generateIRQHandler(6, MCUAL_USART6, USART6);
#endif
#ifdef CONFIG_MCUAL_USART_7
generateIRQHandler(7, MCUAL_USART7, UART7);
#endif
#ifdef CONFIG_MCUAL_USART_8
generateIRQHandler(8, MCUAL_USART8, UART8);
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

static int mcual_usart_get_irq_id(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_1
    case MCUAL_USART1:
      return USART1_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_2
    case MCUAL_USART2:
      return USART2_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_3
    case MCUAL_USART3:
      return USART3_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_4
    case MCUAL_USART4:
      return UART4_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_5
    case MCUAL_USART5:
      return UART5_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_6
    case MCUAL_USART6:
      return USART6_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_7
    case MCUAL_USART7:
      return UART7_IRQn;
#endif

#ifdef CONFIG_MCUAL_USART_8
    case MCUAL_USART8:
      return UART8_IRQn;
#endif
  }

  return 0;
}

void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  tx_queues[usart_id] = xQueueCreate(mcual_usart_get_tx_buffer_size(usart_id), sizeof(uint8_t));
  rx_queues[usart_id] = xQueueCreate(mcual_usart_get_rx_buffer_size(usart_id), sizeof(uint8_t));
#else
  switch(usart_id)
  {
  #ifdef CONFIG_MCUAL_USART_1
    case MCUAL_USART1:
      tx_buffer[usart_id] = tx_buffer_1;
      rx_buffer[usart_id] = rx_buffer_1;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_2
    case MCUAL_USART2:
      tx_buffer[usart_id] = tx_buffer_2;
      rx_buffer[usart_id] = rx_buffer_2;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_3
    case MCUAL_USART3:
      tx_buffer[usart_id] = tx_buffer_3;
      rx_buffer[usart_id] = rx_buffer_3;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_4
    case MCUAL_USART4:
      tx_buffer[usart_id] = tx_buffer_4;
      rx_buffer[usart_id] = rx_buffer_4;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_5
    case MCUAL_USART5:
      tx_buffer[usart_id] = tx_buffer_5;
      rx_buffer[usart_id] = rx_buffer_5;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_6
    case MCUAL_USART6:
      tx_buffer[usart_id] = tx_buffer_6;
      rx_buffer[usart_id] = rx_buffer_6;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_7
    case MCUAL_USART7:
      tx_buffer[usart_id] = tx_buffer_7;
      rx_buffer[usart_id] = rx_buffer_7;
      break;
  #endif
  
  #ifdef CONFIG_MCUAL_USART_8
    case MCUAL_USART8:
      tx_buffer[usart_id] = tx_buffer_8;
      rx_buffer[usart_id] = rx_buffer_8;
      break;
  #endif
  }

  tx_buffer_read[usart_id] = 0;
  tx_buffer_write[usart_id] = 0;
  rx_buffer_read[usart_id] = 0;
  rx_buffer_write[usart_id] = 0;
#endif

  USART_TypeDef * reg = mcual_usart_get_register(usart_id);
  mcual_clock_id_t clock = MCUAL_CLOCK_PERIPHERAL_1;

  //enable clock
  switch(usart_id)
  {
    case MCUAL_USART1:
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

 // pres_i = 0x2480;
  //set baudrate
  reg->BRR = pres_i;

  //reset SPI parameters and hardware flow control
  reg->CR2 = 0;
  reg->CR3 = 0;
  
  //enable usart tx and rx
  reg->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
  
  int irq_id = mcual_usart_get_irq_id(usart_id);
  NVIC->IP[irq_id] = 15 << 4;
  NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
}



void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte)
{
  USART_TypeDef * reg = mcual_usart_get_register(usart_id);
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  xQueueSend(tx_queues[usart_id], &byte, portMAX_DELAY);
#else
  int done = 0;
  while(!done)
  {

    done = 0;
    int32_t next_write = tx_buffer_write[usart_id] + 1;

    if(next_write >= mcual_usart_get_tx_buffer_size(usart_id))
    {
      next_write = 0;
    }

    if(next_write != tx_buffer_read[usart_id])
    {
      __disable_irq();
      volatile uint8_t * buffer = tx_buffer[usart_id];
      buffer[tx_buffer_write[usart_id]] = byte;
      tx_buffer_write[usart_id] = next_write;
      done = 1;
      __enable_irq();
    }
    else
    {
      reg->CR1 |= USART_CR1_TXEIE;
    }
  }
#endif

  reg->CR1 |= USART_CR1_TXEIE;
}

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id)
{
  uint8_t byte;
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  xQueueReceive(rx_queues[usart_id], &byte, portMAX_DELAY);
#else
  int16_t r = -1;
  while(r == -1)
  {
    r = mcual_usart_recv_no_wait(usart_id);
  }
  byte = r;
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
  int16_t byte = -1;
  __disable_irq();
  if(rx_buffer_read[usart_id] != rx_buffer_write[usart_id])
  {
    byte = rx_buffer[usart_id][rx_buffer_read[usart_id]];
    rx_buffer_read[usart_id] += 1;
    if(rx_buffer_read[usart_id] >= mcual_usart_get_rx_buffer_size(usart_id))
    {
      rx_buffer_read[usart_id] = 0;
    }
  }
  __enable_irq();
  return byte;
#endif
}

#endif
