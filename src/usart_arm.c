#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_USART

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

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

void mcual_usart_init(mcual_usart_id_t usart_id, uint32_t baudrate)
{
  USART_TypeDef * reg = mcual_usart_get_register(usart_id);

  //enable clock
  switch(usart_id)
  {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
      break;

    case 4:
      RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
      break;

    case 5:
      RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
      break;

    case 6:
      RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
      break;

    case 7:
      RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
      break;

    case 8:
      RCC->APB1ENR |= RCC_APB1ENR_UART8EN;
      break;
  }

  //compute baudrate
  float pres = (float)(mcual_clock_get_APB_peripheral_clock_Hz()) / (baudrate * 16.0);
  int pres_i = (pres * 16.0);

  //set baudrate
  reg->BRR = pres_i;

  //reset SPI parameters and hardware flow control
  reg->CR2 = 0;
  reg->CR3 = 0;
  
  //enable usart tx and rx
  reg->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

#endif
