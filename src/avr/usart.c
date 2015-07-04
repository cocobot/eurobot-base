#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_USART

#include <stdlib.h>
#include <mcual.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
#error not implemented yet
#else

#ifdef CONFIG_MCUAL_USART_0
volatile uint16_t mcual_usart_0_rx_write; 
volatile uint16_t mcual_usart_0_rx_read; 
volatile uint16_t mcual_usart_0_tx_write; 
volatile uint16_t mcual_usart_0_tx_read; 
#else
# define CONFIG_MCUAL_USART_0_RX_SIZE 0
# define CONFIG_MCUAL_USART_0_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_1
volatile uint16_t mcual_usart_1_rx_write; 
volatile uint16_t mcual_usart_1_rx_read; 
volatile uint16_t mcual_usart_1_tx_write; 
volatile uint16_t mcual_usart_1_tx_read; 
#else
# define CONFIG_MCUAL_USART_1_RX_SIZE 0
# define CONFIG_MCUAL_USART_1_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_2
volatile uint16_t mcual_usart_2_rx_write; 
volatile uint16_t mcual_usart_2_rx_read; 
volatile uint16_t mcual_usart_2_tx_write; 
volatile uint16_t mcual_usart_2_tx_read; 
#else
# define CONFIG_MCUAL_USART_2_RX_SIZE 0
# define CONFIG_MCUAL_USART_2_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_3
volatile uint16_t mcual_usart_3_rx_write; 
volatile uint16_t mcual_usart_3_rx_read; 
volatile uint16_t mcual_usart_3_tx_write; 
volatile uint16_t mcual_usart_3_tx_read; 
#else
# define CONFIG_MCUAL_USART_3_RX_SIZE 0
# define CONFIG_MCUAL_USART_3_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_4
volatile uint16_t mcual_usart_4_rx_write; 
volatile uint16_t mcual_usart_4_rx_read; 
volatile uint16_t mcual_usart_4_tx_write; 
volatile uint16_t mcual_usart_4_tx_read; 
#else
# define CONFIG_MCUAL_USART_4_RX_SIZE 0
# define CONFIG_MCUAL_USART_4_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_5
volatile uint16_t mcual_usart_5_rx_write; 
volatile uint16_t mcual_usart_5_rx_read; 
volatile uint16_t mcual_usart_5_tx_write; 
volatile uint16_t mcual_usart_5_tx_read; 
#else
# define CONFIG_MCUAL_USART_5_RX_SIZE 0
# define CONFIG_MCUAL_USART_5_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_6
volatile uint16_t mcual_usart_6_rx_write; 
volatile uint16_t mcual_usart_6_rx_read; 
volatile uint16_t mcual_usart_6_tx_write; 
volatile uint16_t mcual_usart_6_tx_read; 
#else
# define CONFIG_MCUAL_USART_6_RX_SIZE 0
# define CONFIG_MCUAL_USART_6_TX_SIZE 0
#endif
#ifdef CONFIG_MCUAL_USART_7
volatile uint16_t mcual_usart_7_rx_write; 
volatile uint16_t mcual_usart_7_rx_read; 
volatile uint16_t mcual_usart_7_tx_write; 
volatile uint16_t mcual_usart_7_tx_read; 
#else
# define CONFIG_MCUAL_USART_7_RX_SIZE 0
# define CONFIG_MCUAL_USART_7_TX_SIZE 0
#endif


#define MCUAL_USART_0_RX_START  (0)
#define MCUAL_USART_1_RX_START  (CONFIG_MCUAL_USART_0_RX_SIZE)
#define MCUAL_USART_2_RX_START  (MCUAL_USART_1_RX_START + CONFIG_MCUAL_USART_1_RX_SIZE)
#define MCUAL_USART_3_RX_START  (MCUAL_USART_2_RX_START + CONFIG_MCUAL_USART_2_RX_SIZE)
#define MCUAL_USART_4_RX_START  (MCUAL_USART_3_RX_START + CONFIG_MCUAL_USART_3_RX_SIZE)
#define MCUAL_USART_5_RX_START  (MCUAL_USART_4_RX_START + CONFIG_MCUAL_USART_4_RX_SIZE)
#define MCUAL_USART_6_RX_START  (MCUAL_USART_5_RX_START + CONFIG_MCUAL_USART_5_RX_SIZE)
#define MCUAL_USART_7_RX_START  (MCUAL_USART_6_RX_START + CONFIG_MCUAL_USART_6_RX_SIZE)
#define MCUAL_USART_RX_TOTAL    (MCUAL_USART_7_RX_START + CONFIG_MCUAL_USART_7_RX_SIZE)
#define MCUAL_USART_0_TX_START  (0)
#define MCUAL_USART_1_TX_START  (CONFIG_MCUAL_USART_0_TX_SIZE)
#define MCUAL_USART_2_TX_START  (MCUAL_USART_1_TX_START + CONFIG_MCUAL_USART_1_TX_SIZE)
#define MCUAL_USART_3_TX_START  (MCUAL_USART_2_TX_START + CONFIG_MCUAL_USART_2_TX_SIZE)
#define MCUAL_USART_4_TX_START  (MCUAL_USART_3_TX_START + CONFIG_MCUAL_USART_3_TX_SIZE)
#define MCUAL_USART_5_TX_START  (MCUAL_USART_4_TX_START + CONFIG_MCUAL_USART_4_TX_SIZE)
#define MCUAL_USART_6_TX_START  (MCUAL_USART_5_TX_START + CONFIG_MCUAL_USART_5_TX_SIZE)
#define MCUAL_USART_7_TX_START  (MCUAL_USART_6_TX_START + CONFIG_MCUAL_USART_6_TX_SIZE)
#define MCUAL_USART_TX_TOTAL    (MCUAL_USART_7_TX_START + CONFIG_MCUAL_USART_7_TX_SIZE)

#define generateUSART(num, port) \
void mcual_usart_##num##_send(uint8_t byte)\
{\
  uint8_t nx_write_ptr = mcual_usart_##num##_tx_write + 1;\
  if(nx_write_ptr >= CONFIG_MCUAL_USART_##num##_TX_SIZE)\
  {\
    nx_write_ptr = 0;\
  }\
  while (nx_write_ptr == mcual_usart_##num##_tx_read)\
  {\
    USART##port.CTRLA |= USART_DREINTLVL_MED_gc;\
  }\
  tx_buffer[nx_write_ptr + MCUAL_USART_##num##_TX_START] = byte;\
  mcual_usart_##num##_tx_write = nx_write_ptr;\
  USART##port.CTRLA |= USART_DREINTLVL_MED_gc;\
}\
ISR(USART##port##_DRE_vect)\
{\
  if (mcual_usart_##num##_tx_read == mcual_usart_##num##_tx_write)\
  {\
    USART##port.CTRLA &= ~USART_DREINTLVL_gm;\
  }\
  else\
  {\
    uint8_t nx_read_ptr = mcual_usart_##num##_tx_read + 1;\
    if(nx_read_ptr >= CONFIG_MCUAL_USART_##num##_TX_SIZE)\
    {\
      nx_read_ptr = 0;\
    }\
    USART##port.DATA = tx_buffer[mcual_usart_##num##_tx_read + MCUAL_USART_##num##_TX_START];\
    mcual_usart_##num##_tx_read = nx_read_ptr;\
  }\
}


uint8_t rx_buffer[MCUAL_USART_RX_TOTAL];
uint8_t tx_buffer[MCUAL_USART_TX_TOTAL];
#endif


static USART_t * mcual_usart_get_register(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
    case MCUAL_USART0:
      return &USARTC0;

    case MCUAL_USART1:
      return &USARTC1;

    case MCUAL_USART2:
      return &USARTD0;

    case MCUAL_USART3:
      return &USARTD1;

    case MCUAL_USART4:
      return &USARTE0;

    case MCUAL_USART5:
      return &USARTE1;

    case MCUAL_USART6:
      return &USARTF0;

    case MCUAL_USART7:
      return &USARTF1;
  }

  return NULL;
}

#ifndef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
static inline void mcual_usart_queue_init(mcual_usart_id_t usart_id)
{
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_0
    case MCUAL_USART0:
      mcual_usart_0_rx_write = 0;
      mcual_usart_0_rx_read = 0;
      mcual_usart_0_tx_write = 0;
      mcual_usart_0_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_1
    case MCUAL_USART1:
      mcual_usart_1_rx_write = 0;
      mcual_usart_1_rx_read = 0;
      mcual_usart_1_tx_write = 0;
      mcual_usart_1_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_2
    case MCUAL_USART2:
      mcual_usart_2_rx_write = 0;
      mcual_usart_2_rx_read = 0;
      mcual_usart_2_tx_write = 0;
      mcual_usart_2_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_3
    case MCUAL_USART3:
      mcual_usart_3_rx_write = 0;
      mcual_usart_3_rx_read = 0;
      mcual_usart_3_tx_write = 0;
      mcual_usart_3_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_4
    case MCUAL_USART4:
      mcual_usart_4_rx_write = 0;
      mcual_usart_4_rx_read = 0;
      mcual_usart_4_tx_write = 0;
      mcual_usart_4_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_5
    case MCUAL_USART5:
      mcual_usart_5_rx_write = 0;
      mcual_usart_5_rx_read = 0;
      mcual_usart_5_tx_write = 0;
      mcual_usart_5_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_6
    case MCUAL_USART6:
      mcual_usart_6_rx_write = 0;
      mcual_usart_6_rx_read = 0;
      mcual_usart_6_tx_write = 0;
      mcual_usart_6_tx_read = 0;
      break;
#endif

#ifdef CONFIG_MCUAL_USART_7
    case MCUAL_USART7:
      mcual_usart_7_rx_write = 0;
      mcual_usart_7_rx_read = 0;
      mcual_usart_7_tx_write = 0;
      mcual_usart_7_tx_read = 0;
      break;
#endif

  }
}
#endif

void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
#error not implemented yet
#else
  mcual_usart_queue_init(usart_id);
#endif
  USART_t * reg = mcual_usart_get_register(usart_id);

  uint32_t baudrate_raw = mcual_clock_get_frequency_Hz(MCUAL_CLOCK_SYSTEM) / (16UL * baudrate) - 1;

  reg->BAUDCTRLB = (uint8_t)(baudrate_raw >> 8) & 0x0F;
  reg->BAUDCTRLA = (uint8_t)baudrate_raw;
  reg->CTRLC = USART_CHSIZE_8BIT_gc;
  reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
  reg->CTRLA = 0; //USART_RXCINTLVL_MED_gc;
}

#ifdef CONFIG_MCUAL_USART_0
generateUSART(0,C0);
#endif
#ifdef CONFIG_MCUAL_USART_1
generateUSART(1,C1);
#endif
#ifdef CONFIG_MCUAL_USART_2
generateUSART(2,D0);
#endif
#ifdef CONFIG_MCUAL_USART_3
generateUSART(3,D1);
#endif
#ifdef CONFIG_MCUAL_USART_4
generateUSART(4,E0);
#endif
#ifdef CONFIG_MCUAL_USART_5
generateUSART(5,E1);
#endif
#ifdef CONFIG_MCUAL_USART_6
generateUSART(6,F0);
#endif
#ifdef CONFIG_MCUAL_USART_7
generateUSART(7,F1);
#endif




void mcual_usart_send(mcual_usart_id_t usart_id, uint8_t byte)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  (void)usart_id;
  (void)byte;
#error not implemented yet
#else
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_0
    case MCUAL_USART0:
      mcual_usart_0_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_1
    case MCUAL_USART1:
      mcual_usart_1_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_2
    case MCUAL_USART2:
      mcual_usart_2_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_3
    case MCUAL_USART3:
      mcual_usart_3_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_4
    case MCUAL_USART4:
      mcual_usart_4_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_5
    case MCUAL_USART5:
      mcual_usart_5_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_6
    case MCUAL_USART6:
      mcual_usart_6_send(byte);
      break;
#endif

#ifdef CONFIG_MCUAL_USART_7
    case MCUAL_USART7:
      mcual_usart_7_send(byte);
      break;
#endif
  }
#endif
}

uint8_t mcual_usart_recv(mcual_usart_id_t usart_id)
{
  uint8_t byte;
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
#error not implemented yet
#else
  int16_t r = -1;
  while(r == -1)
  {
    r = mcual_usart_recv_no_wait(usart_id);
  }
  byte = r & 0xff;
#endif
  return byte;
}

int16_t mcual_usart_recv_no_wait(mcual_usart_id_t usart_id)
{
#ifdef CONFIG_MCUAL_USART_USE_FREERTOS_QUEUES
  (void)usart_id;
#error not implemented yet
#else
  switch(usart_id)
  {
#ifdef CONFIG_MCUAL_USART_0
    case MCUAL_USART0:
      return mcual_usart_0_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_1
    case MCUAL_USART1:
      return mcual_usart_1_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_2
    case MCUAL_USART2:
      return mcual_usart_2_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_3
    case MCUAL_USART3:
      return mcual_usart_3_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_4
    case MCUAL_USART4:
      return mcual_usart_4_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_5
    case MCUAL_USART5:
      return mcual_usart_5_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_6
    case MCUAL_USART6:
      return mcual_usart_6_recv_no_wait();
      break;
#endif

#ifdef CONFIG_MCUAL_USART_7
    case MCUAL_USART7:
      return mcual_usart_7_recv_no_wait();
      break;
#endif
  }
#endif
  return -1;
}

#endif
