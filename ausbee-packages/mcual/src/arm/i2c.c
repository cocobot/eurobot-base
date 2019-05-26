#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_I2C

#include <mcual.h>
#include <stm32f4xx.h>
#include <string.h>

//static I2C_TypeDef * mcual_i2c_get_register(mcual_i2c_id_t i2c_id)
//{
//  switch(i2c_id)
//  {
//    case MCUAL_I2C1:
//      return I2C1;
//
//    case MCUAL_I2C2:
//      return I2C2;
//
//    case MCUAL_I2C3:
//      return I2C3;
//
//  }
//  return NULL;
//}
//
//static int mcual_i2c_get_irq_ev_id(mcual_i2c_id_t i2c_id)
//{
//  switch(i2c_id)
//  {
//    case MCUAL_I2C1:
//      return I2C1_EV_IRQn;
//
//    case MCUAL_I2C2:
//      return I2C2_EV_IRQn;
//
//    case MCUAL_I2C3:
//      return I2C3_EV_IRQn;
//  }
//
//  return 0;
//}
//
//static int mcual_i2c_get_irq_err_id(mcual_i2c_id_t i2c_id)
//{
//  switch(i2c_id)
//  {
//    case MCUAL_I2C1:
//      return I2C1_ER_IRQn;
//
//    case MCUAL_I2C2:
//      return I2C2_ER_IRQn;
//
//    case MCUAL_I2C3:
//      return I2C3_ER_IRQn;
//  }
//
//  return 0;
//}
//
//void mcual_i2c_master_init(mcual_i2c_id_t i2c_id, uint32_t frequency_Hz)
//{
//  I2C_TypeDef * reg = mcual_i2c_get_register(i2c_id);
//  mcual_clock_id_t clock = MCUAL_CLOCK_PERIPHERAL_1;
//
//  switch(i2c_id)
//  {
//    case MCUAL_I2C1:
//      RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
//      clock = MCUAL_CLOCK_PERIPHERAL_1;
//      break;
//
//    case MCUAL_I2C2:
//      RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
//      clock = MCUAL_CLOCK_PERIPHERAL_1;
//      break;
//
//    case MCUAL_I2C3:
//      RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
//      clock = MCUAL_CLOCK_PERIPHERAL_1;
//      break;
//  }
//
//  //Reset i2c
//  reg->CR1 = I2C_CR1_SWRST;
//  reg->CR1 = 0;
//
//  uint32_t clk_Hz = mcual_clock_get_frequency_Hz(clock);
//  (void)clk_Hz;
//  (void)frequency_Hz;
//
//  frequency_Hz = 100000U;
//  //reg->CCR = clk_Hz / (2 * frequency_Hz);
//  //reg->CR2 = clk_Hz / 1000000U;
//  //reg->TRISE = clk_Hz / 1000000U + 1;
//  reg->CCR = 50;
//  reg->CR2 = 10;
//  reg->TRISE = 68;
//  
//
//  reg->CR1 = I2C_CR1_POS | I2C_CR1_PE;
//
//  reg->SR1 = 0;
//
//  int irq_id = mcual_i2c_get_irq_ev_id(i2c_id);
//  NVIC->IP[irq_id] = 13 << 4;
//  //NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
//  irq_id = mcual_i2c_get_irq_err_id(i2c_id);
//  NVIC->IP[irq_id] = 13 << 4;
//  //NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
//
//  __enable_irq();
//}
//
//static void mcual_i2c_stop(I2C_TypeDef * reg)
//{
//  reg->CR1 = I2C_CR1_STOP | I2C_CR1_PE;
//  while(reg->SR1 & I2C_SR2_BUSY);
//  reg->SR1 = 0;
//}
//
//mcual_i2c_status_t mcual_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
//{
//  I2C_TypeDef * reg = mcual_i2c_get_register(id);
//
//  (void)addr;
//  (void)txbuf;
//  (void)tx_size;
//  (void)rxbuf;
//  (void)rx_size;
//
//  while(reg->SR2 & I2C_SR2_BUSY);
//  reg->SR1 = 0;
//
//  reg->CR1 = I2C_CR1_START | I2C_CR1_PE;
//
//  volatile unsigned int sr;
//  while(!((sr = reg->SR1) & I2C_SR1_SB));
//  reg->DR = (addr << 1);
//
//  do
//  {
//    sr = reg->SR1;
//    if(sr & I2C_SR1_AF)
//    {
//      mcual_i2c_stop(reg);
//      return MCUAL_I2C_FAIL;
//    }
//
//  }while(!(sr & I2C_SR1_ADDR));
//
//  sr = reg->SR1;
//  sr = reg->SR2;
//
//  int i = 0;
//  for(i = 0; i < tx_size; i += 1)
//  { 
//
//    reg->DR = *txbuf;
//    do
//    {
//      sr = reg->SR1;
//      if(sr & I2C_SR1_AF)
//      {
//        mcual_i2c_stop(reg);
//        return MCUAL_I2C_FAIL;
//      }
//    }
//    while(!(sr & I2C_SR1_BTF));
//
//    txbuf += 1;
//
//  }
//  //while(!(reg->SR1 & I2C_SR1_BTF));
//
//  mcual_i2c_stop(reg);
//
//  return 0;
//}
//
////void I2C1_EV_IRQHandler(void)
////{
////  volatile unsigned int sr1 = I2C1->SR1;
////  volatile unsigned int sr2 = I2C1->SR2;
////
////  (void)sr1;
////  (void)sr2;
////
////  switch(i2c1_state)
////  {
////    case MCUAL_I2C_STATE_START:
////      if(sr1 & I2C_SR1_SB)
////      {
////        I2C1->DR = i2c1_addr << 1;
////        i2c1_state = MCUAL_I2C_STATE_SEND_ADDR;
////        return;
////      }
////      break;
////
////    case MCUAL_I2C_STATE_SEND_ADDR:
////      if(sr1 & I2C_SR1_ADDR)
////      {
////        mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
////        I2C1->DR = 0x12;
////        i2c1_state = MCUAL_I2C_STATE_OK;
////        return;
////      }
////      break;
////
////    case MCUAL_I2C_STATE_OK:
////    case MCUAL_I2C_STATE_ERROR:
////      break;
////  }
////
////  if(i2c1_state != MCUAL_I2C_STATE_OK)
////  {
////    i2c1_state = MCUAL_I2C_STATE_ERROR;
////  }
////  volatile unsigned int cr1 = I2C1->CR1;
////  cr1 &= ~I2C_CR1_START;
////  I2C1->CR1 = cr1 | I2C_CR1_STOP;
////}
//////void I2C1_ER_IRQHandler(void)
//////{
//////  volatile unsigned int sr1 = I2C1->SR1;
//////  volatile unsigned int sr2 = I2C1->SR2;
//////
//////  (void)sr1;
//////  (void)sr2;
//////  volatile unsigned int cr1 = I2C1->CR1;
//////  cr1 &= ~I2C_CR1_START;
//////  I2C1->CR1 = cr1 | I2C_CR1_STOP;
//////
//////  mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
//////}
//////
////////generateIRQHandler(1, MCUAL_I2C1, I2C1);
////////generateIRQHandler(2, MCUAL_I2C2, I2C2);
////////generateIRQHandler(3, MCUAL_I2C3, I2C3);
#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_I2C

#include <mcual.h>
#include <stm32f4xx.h>
#include <string.h>

#ifdef CONFIG_MCUAL_I2C_USE_FREERTOS 
#include <FreeRTOS.h>
#include <queue.h>
#include <event_groups.h>
#endif

#define MCUAL_I2C_NUMBER (MCUAL_I2C3 + 1)

#ifdef CONFIG_MCUAL_I2C_USE_FREERTOS
static QueueHandle_t tx_queues[MCUAL_I2C_NUMBER];
static EventGroupHandle_t transfer_done[MCUAL_I2C_NUMBER];
#endif

static I2C_TypeDef * mcual_i2c_get_register(mcual_i2c_id_t i2c_id)
{
  switch(i2c_id)
  {
    case MCUAL_I2C1:
      return I2C1;

    case MCUAL_I2C2:
      return I2C2;

    case MCUAL_I2C3:
      return I2C3;

  }
  return NULL;
}

static int mcual_i2c_get_irq_ev_id(mcual_i2c_id_t i2c_id)
{
  switch(i2c_id)
  {
    case MCUAL_I2C1:
      return I2C1_EV_IRQn;

    case MCUAL_I2C2:
      return I2C2_EV_IRQn;

    case MCUAL_I2C3:
      return I2C3_EV_IRQn;
  }

  return 0;
}

static int mcual_i2c_get_irq_err_id(mcual_i2c_id_t i2c_id)
{
  switch(i2c_id)
  {
    case MCUAL_I2C1:
      return I2C1_ER_IRQn;

    case MCUAL_I2C2:
      return I2C2_ER_IRQn;

    case MCUAL_I2C3:
      return I2C3_ER_IRQn;
  }

  return 0;
}


#ifdef CONFIG_MCUAL_I2C_USE_FREERTOS
static int mcual_i2c_get_tx_buffer_size(mcual_i2c_id_t i2c_id)
{
  (void)i2c_id;
  return 8;
}
#endif


void mcual_i2c_master_init(mcual_i2c_id_t i2c_id, uint32_t frequency_Hz)
{
#ifdef CONFIG_MCUAL_I2C_USE_FREERTOS
  tx_queues[i2c_id] = xQueueCreate(mcual_i2c_get_tx_buffer_size(i2c_id), sizeof(uint8_t));
  transfer_done[i2c_id] = xEventGroupCreate();
#endif
  
  I2C_TypeDef * reg = mcual_i2c_get_register(i2c_id);
  mcual_clock_id_t clock = MCUAL_CLOCK_PERIPHERAL_1;

  switch(i2c_id)
  {
    case MCUAL_I2C1:
      RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case MCUAL_I2C2:
      RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;

    case MCUAL_I2C3:
      RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
      clock = MCUAL_CLOCK_PERIPHERAL_1;
      break;
  }

  //Reset i2c
  reg->CR1 = I2C_CR1_SWRST;
  reg->CR1 = 0;

  uint32_t clk_Hz = mcual_clock_get_frequency_Hz(clock);
  (void)clk_Hz;
  (void)frequency_Hz;

  frequency_Hz = 100000U;
  //reg->CCR = clk_Hz / (2 * frequency_Hz);
  //reg->CR2 = clk_Hz / 1000000U;
  //reg->TRISE = clk_Hz / 1000000U + 1;
  reg->CCR = 50;
  reg->CR2 = 10;
  reg->TRISE = 68;
  

  reg->CR1 = I2C_CR1_PE;

  reg->SR1 = 0;

  int irq_id = mcual_i2c_get_irq_ev_id(i2c_id);
  NVIC->IP[irq_id] = 13 << 4;
  NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
  irq_id = mcual_i2c_get_irq_err_id(i2c_id);
  NVIC->IP[irq_id] = 13 << 4;
  NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
}

#ifdef CONFIG_MCUAL_I2C_USE_FREERTOS
static void mcual_i2c_stop(mcual_i2c_id_t id, int success)
{
  I2C_TypeDef * reg = mcual_i2c_get_register(id);

  reg->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
  I2C1->SR1 = 0;
  I2C1->SR2 = 0;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t xResult = pdFALSE;
  if(success)
  {
    xResult =  xEventGroupSetBitsFromISR(transfer_done[id], 1 << MCUAL_I2C_SUCCESS, &xHigherPriorityTaskWoken);
  }
  else
  {
    xResult = xEventGroupSetBitsFromISR(transfer_done[id], 1 << MCUAL_I2C_FAIL, &xHigherPriorityTaskWoken);
  }

  if(xResult != pdFAIL)
  {
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }
}

mcual_i2c_status_t mcual_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
{
  (void)rxbuf;
  (void)rx_size;

  I2C_TypeDef * reg = mcual_i2c_get_register(id);
  int i;

  addr <<= 1;

  xEventGroupClearBits(transfer_done[id], 0xFF);

  xQueueReset(tx_queues[id]);
  xQueueSend(tx_queues[id], &addr, portMAX_DELAY);
  for(i = 0; i < tx_size; i += 1)
  {
    xQueueSend(tx_queues[id], txbuf + i, portMAX_DELAY);
  }

  reg->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
  reg->CR1 = I2C_CR1_START | I2C_CR1_PE;

  EventBits_t result = xEventGroupWaitBits(transfer_done[id], 0xFF, pdFALSE, pdFALSE, 1000 / portTICK_PERIOD_MS);

  I2C1->CR1 = I2C_CR1_STOP;

  if(result & (1 << MCUAL_I2C_SUCCESS))
  {
    return MCUAL_I2C_SUCCESS;
  }

  return MCUAL_I2C_FAIL;
}

void I2C1_EV_IRQHandler(void)
{
  volatile unsigned int sr1 = I2C1->SR1;
  volatile unsigned int sr2 = I2C1->SR2;
  (void)sr1;
  (void)sr2;

  uint8_t byte;

  if(sr1 & (I2C_SR1_SB | I2C_SR1_ADDR | I2C_SR1_BTF))
  {
    if(xQueueReceiveFromISR(tx_queues[1], &byte, NULL) == pdTRUE)
    {
      I2C1->DR = byte;
    }
    else if(!(sr1 & (I2C_SR1_STOPF)))
    {
      mcual_i2c_stop(1, 1);
    }
  }
  //else if(!(sr1 & (I2C_SR1_STOPF)))
  //{
  //  if(sr1 == 0)
  //  {
  //    mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
  //  }
  //  mcual_i2c_stop(1, 0);
  //}
}

void I2C1_ER_IRQHandler(void)
{
  volatile unsigned int sr1 = I2C1->SR1;
  volatile unsigned int sr2 = I2C1->SR2;

  (void)sr1;
  (void)sr2;

  if(!(sr1 & (I2C_SR1_STOPF)))
  {
    mcual_i2c_stop(1, 0);
  }
}
#endif

#endif

#endif
