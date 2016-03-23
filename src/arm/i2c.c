#include <mcual.h>
#include <stm32f4xx.h>
#include <string.h>

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

void mcual_i2c_master_init(mcual_i2c_id_t i2c_id, uint32_t frequency_Hz)
{
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
  

  reg->CR1 = I2C_CR1_POS | I2C_CR1_PE;

  reg->SR1 = 0;

  int irq_id = mcual_i2c_get_irq_ev_id(i2c_id);
  NVIC->IP[irq_id] = 13 << 4;
  //NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
  irq_id = mcual_i2c_get_irq_err_id(i2c_id);
  NVIC->IP[irq_id] = 13 << 4;
  //NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));

  __enable_irq();
}

static void mcual_i2c_stop(I2C_TypeDef * reg)
{
  reg->CR1 = I2C_CR1_STOP | I2C_CR1_PE;
  while(reg->SR1 & I2C_SR2_BUSY);
  reg->SR1 = 0;
}

mcual_i2c_status_t mcual_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size)
{
  I2C_TypeDef * reg = mcual_i2c_get_register(id);

  (void)addr;
  (void)txbuf;
  (void)tx_size;
  (void)rxbuf;
  (void)rx_size;

  while(reg->SR2 & I2C_SR2_BUSY);
  reg->SR1 = 0;

  reg->CR1 = I2C_CR1_START | I2C_CR1_PE;

  volatile unsigned int sr;
  while(!((sr = reg->SR1) & I2C_SR1_SB));
  reg->DR = (addr << 1);

  do
  {
    sr = reg->SR1;
    mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
    if(sr & I2C_SR1_AF)
    {
      mcual_i2c_stop(reg);
      mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
      return MCUAL_I2C_FAIL;
    }

  }while(!(sr & I2C_SR1_ADDR));

  sr = reg->SR1;
  sr = reg->SR2;

  int i = 0;
  for(i = 0; i < tx_size; i += 1)
  { 

    reg->DR = *txbuf;
    do
    {
      sr = reg->SR1;
      if(sr & I2C_SR1_AF)
      {
        mcual_i2c_stop(reg);
        mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
        return MCUAL_I2C_FAIL;
      }
    }
    while(!(sr & I2C_SR1_BTF));

    txbuf += 1;

  }
  //while(!(reg->SR1 & I2C_SR1_BTF));

  mcual_gpio_toogle(MCUAL_GPIOC, MCUAL_GPIO_PIN4);
  mcual_i2c_stop(reg);

  return 0;
}

//void I2C1_EV_IRQHandler(void)
//{
//  volatile unsigned int sr1 = I2C1->SR1;
//  volatile unsigned int sr2 = I2C1->SR2;
//
//  (void)sr1;
//  (void)sr2;
//
//  switch(i2c1_state)
//  {
//    case MCUAL_I2C_STATE_START:
//      if(sr1 & I2C_SR1_SB)
//      {
//        I2C1->DR = i2c1_addr << 1;
//        i2c1_state = MCUAL_I2C_STATE_SEND_ADDR;
//        return;
//      }
//      break;
//
//    case MCUAL_I2C_STATE_SEND_ADDR:
//      if(sr1 & I2C_SR1_ADDR)
//      {
//        mcual_gpio_set(MCUAL_GPIOB, MCUAL_GPIO_PIN12);
//        I2C1->DR = 0x12;
//        i2c1_state = MCUAL_I2C_STATE_OK;
//        return;
//      }
//      break;
//
//    case MCUAL_I2C_STATE_OK:
//    case MCUAL_I2C_STATE_ERROR:
//      break;
//  }
//
//  if(i2c1_state != MCUAL_I2C_STATE_OK)
//  {
//    i2c1_state = MCUAL_I2C_STATE_ERROR;
//  }
//  volatile unsigned int cr1 = I2C1->CR1;
//  cr1 &= ~I2C_CR1_START;
//  I2C1->CR1 = cr1 | I2C_CR1_STOP;
//}
////void I2C1_ER_IRQHandler(void)
////{
////  volatile unsigned int sr1 = I2C1->SR1;
////  volatile unsigned int sr2 = I2C1->SR2;
////
////  (void)sr1;
////  (void)sr2;
////  volatile unsigned int cr1 = I2C1->CR1;
////  cr1 &= ~I2C_CR1_START;
////  I2C1->CR1 = cr1 | I2C_CR1_STOP;
////
////  mcual_gpio_toogle(MCUAL_GPIOB, MCUAL_GPIO_PIN5);
////}
////
//////generateIRQHandler(1, MCUAL_I2C1, I2C1);
//////generateIRQHandler(2, MCUAL_I2C2, I2C2);
//////generateIRQHandler(3, MCUAL_I2C3, I2C3);
