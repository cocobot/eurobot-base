#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_GPIO

#include "../arm/gpio.c"

/*
#include <mcual.h>
#include <stm32l4xx_hal.h>

static mcual_gpio_interrupt_handler_t _itr_handlers[16];

static GPIO_TypeDef * mcual_gpio_get_register(mcual_gpio_port_t port)
{
  switch(port)
  {
    case MCUAL_GPIOA:
      return GPIOA;

    case MCUAL_GPIOB:
      return GPIOB;

    case MCUAL_GPIOC:
      return GPIOC;

    case MCUAL_GPIOD:
      return GPIOD;

    case MCUAL_GPIOE:
      return GPIOE;

    case MCUAL_GPIOF:
      return GPIOF;

    case MCUAL_GPIOG:
      return GPIOG;

    case MCUAL_GPIOH:
      return GPIOH;

    case MCUAL_GPIOI:
      return GPIOI;

#ifdef GPIOJ
    case MCUAL_GPIOJ:
      return GPIOJ;
#endif

#ifdef GPIOK
    case MCUAL_GPIOK:
      return GPIOK;
#endif
  }

  return NULL;
}

void mcual_gpio_init(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_direction_t direction)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  switch(direction)
  {
    case MCUAL_GPIO_INPUT:
      GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
      break;

    case MCUAL_GPIO_OUTPUT:
      GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
      break;
  }
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      GPIO_InitStructure.Pin   = (1 << i);
      HAL_GPIO_Init(mcual_gpio_get_register(port), &GPIO_InitStructure);
    }
  }
}

void mcual_gpio_set(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      HAL_GPIO_WritePin(mcual_gpio_get_register(port), (1 << i), GPIO_PIN_SET);
    }
  }
}

void mcual_gpio_clear(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      HAL_GPIO_WritePin(mcual_gpio_get_register(port), (1 << i), GPIO_PIN_RESET);
    }
  }
}

void mcual_gpio_toggle(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      HAL_GPIO_TogglePin(mcual_gpio_get_register(port), (1 << i));
    }
  }
}

uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  uint32_t value = 0;

  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      if(HAL_GPIO_ReadPin(mcual_gpio_get_register(port), (1 << i)))
      {
        value |= (1 << i);
      }
    }
  }

  return value;
}

//void mcual_gpio_set_function(mcual_gpio_port_t port, mcual_gpio_pin_t pin, int function_id)
//{
//  (void)port;
//  (void)pin;
//  (void)function_id;
//}
//
//void mcual_gpio_set_output_type(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_output_type_t type)
//{
//  (void)port;
//  (void)pin;
//  (void)type;
//}

static int mcual_gpio_get_irq_id(mcual_gpio_pin_t pin)
{
  switch(pin)
  {
    case MCUAL_GPIO_PIN0:
      return EXTI0_IRQn;

    case MCUAL_GPIO_PIN1:
      return EXTI1_IRQn;

    case MCUAL_GPIO_PIN2:
      return EXTI2_IRQn;

    case MCUAL_GPIO_PIN3:
      return EXTI3_IRQn;

    case MCUAL_GPIO_PIN4:
      return EXTI4_IRQn;

    case MCUAL_GPIO_PIN5:
    case MCUAL_GPIO_PIN6:
    case MCUAL_GPIO_PIN7:
    case MCUAL_GPIO_PIN8:
    case MCUAL_GPIO_PIN9:
      return EXTI9_5_IRQn;

    case MCUAL_GPIO_PIN10:
    case MCUAL_GPIO_PIN11:
    case MCUAL_GPIO_PIN12:
    case MCUAL_GPIO_PIN13:
    case MCUAL_GPIO_PIN14:
    case MCUAL_GPIO_PIN15:
      return EXTI15_10_IRQn;

  }

  return 0;
}

void mcual_gpio_set_interrupt(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_edge_t edge, mcual_gpio_interrupt_handler_t handler)
{
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      _itr_handlers[i] = handler;
      SYSCFG->EXTICR[i >> 0x02] &= ~(0x0F << (4 * (i & 0x03)));
      SYSCFG->EXTICR[i >> 0x02] |= (port << (4 * (i & 0x03)));

      if((edge == MCUAL_GPIO_RISING_EDGE) || (edge == MCUAL_GPIO_BOTH_EDGE))
      {
        EXTI->RTSR1 |= (1 << i);
      }
      else
      {
        EXTI->RTSR1 &= ~(1 << i);
      }

      if((edge == MCUAL_GPIO_FALLING_EDGE) || (edge == MCUAL_GPIO_BOTH_EDGE))
      {
        EXTI->FTSR1 |= (1 << i);
      }
      else
      {
        EXTI->FTSR1 &= ~(1 << i);
      }

      EXTI->IMR1 |= (1 << i);

      int irq_id = mcual_gpio_get_irq_id((1 << i));
      NVIC->IP[irq_id] = 14 << 4;
      NVIC->ISER[irq_id / 32] |= (1 << (irq_id % 32));
    }
  }
}

#define generateSimpleIRQHandler(pin)  \
void EXTI ## pin ##_IRQHandler(void) \
{\
  if(_itr_handlers[pin] != NULL)\
  {\
    _itr_handlers[pin]();\
  }\
  EXTI->PR1 = (1 << pin);\
}

#define generateComplexIRQHandler(start, end)  \
void EXTI ## end ##_ ## start ## _IRQHandler(void) \
{\
  int i;\
  uint32_t npr = 0;\
  for(i = start; i <= end; i += 1)\
  {\
    if(EXTI->IMR1 & (1 << i))\
    {\
      if(EXTI->PR1 & (1 << i))\
      {\
        if(_itr_handlers[i] != NULL)\
        {\
          _itr_handlers[i]();\
        }\
        npr |= (1 << i);\
      }\
    }\
    EXTI->PR1 = npr;\
  }\
}

generateSimpleIRQHandler(0);
generateSimpleIRQHandler(1);
generateSimpleIRQHandler(2);
generateSimpleIRQHandler(3);
generateSimpleIRQHandler(4);
generateComplexIRQHandler(5, 9);
generateComplexIRQHandler(10, 15);
*/

#endif
