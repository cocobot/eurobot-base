#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_GPIO

#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

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

    case MCUAL_GPIOJ:
      return GPIOJ;

    case MCUAL_GPIOK:
      return GPIOK;
  }

  return NULL;
}

void mcual_gpio_init(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_direction_t direction)
{
  //enable clock
  RCC->AHB1ENR |= (1 << port);

  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  //get registers
  uint32_t moder = reg->MODER;
  uint32_t ospeedr = reg->OSPEEDR;
  uint32_t afrl = reg->AFR[0];
  uint32_t afrh = reg->AFR[1];

  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      if(direction == MCUAL_GPIO_INPUT)
      {
        //set input
        moder &= ~(0x03 << (2 * i));
      }
      else
      {
        //set output
        moder &= ~(0x03 << (2 * i));
        moder |= (0x01 << (2 * i));
      }

      //set high speed
      ospeedr |= (0x03 << (2 * i));

      //set function 0 (GPIO)
      if(i < 8)
      {
        afrl &= ~(0x0F << (4 * i));
      }
      else
      {
        afrh &= ~(0x0F << (4 * (i - 8)));
      }
    }
  }

  //set registers
  reg->MODER = moder;
  reg->OSPEEDR = ospeedr;
  reg->AFR[0] = afrl;
  reg->AFR[1] = afrh;
}

void mcual_gpio_set(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  reg->BSRRL = pin;
}

void mcual_gpio_clear(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  reg->BSRRH = pin;
}

void mcual_gpio_toggle(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  reg->ODR ^= pin;
}

uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  return reg->IDR & pin;
}

void mcual_gpio_set_function(mcual_gpio_port_t port, mcual_gpio_pin_t pin, int function_id)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  uint32_t afrl = reg->AFR[0];
  uint32_t afrh = reg->AFR[1];
  uint32_t moder = reg->MODER;
  
  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      moder &= ~(0x03 << (2 * i));
      if(function_id == MCUAL_GPIO_FUNCTION_ANALOG)
      {
        moder |= (0x03 << (2 * i));
      }
      else
      {
        moder |= (0x02 << (2 * i));

        if(i < 8)
        {
          afrl &= ~(0x0F << (4 * i));
          afrl |=  (function_id << (4 * i));
        }
        else
        {
          afrh &= ~(0x0F << (4 * (i - 8)));
          afrh |=  (function_id << (4 * (i - 8)));
        }
      }
    }
  }

  reg->MODER = moder;
  reg->AFR[0] = afrl;
  reg->AFR[1] = afrh;
}

void mcual_gpio_set_output_type(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_output_type_t type)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);
  int i;
  for(i = 0; i < 16; i += 1)
  {
    if(pin & (1 << i))
    {
      if(type == MCUAL_GPIO_OPEN_DRAIN)
      {
        reg->OTYPER |= (1 << i);
      }
      else
      {
        reg->OTYPER &= ~(1 << i);
      }
    }
  }
}

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
        EXTI->RTSR |= (1 << i);
      }
      else
      {
        EXTI->RTSR &= ~(1 << i);
      }

      if((edge == MCUAL_GPIO_FALLING_EDGE) || (edge == MCUAL_GPIO_BOTH_EDGE))
      {
        EXTI->FTSR |= (1 << i);
      }
      else
      {
        EXTI->FTSR &= ~(1 << i);
      }

      EXTI->IMR |= (1 << i);

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
  EXTI->PR = (1 << pin);\
}

#define generateComplexIRQHandler(start, end)  \
void EXTI ## end ##_ ## start ## _IRQHandler(void) \
{\
  int i;\
  uint32_t npr = 0;\
  for(i = start; i <= end; i += 1)\
  {\
    if(EXTI->IMR & (1 << i))\
    {\
      if(EXTI->PR & (1 << i))\
      {\
        if(_itr_handlers[i] != NULL)\
        {\
          _itr_handlers[i]();\
        }\
        npr |= (1 << i);\
      }\
    }\
    EXTI->PR = npr;\
  }\
}
 //   EXTI->PR = (1 << i);

generateSimpleIRQHandler(0);
generateSimpleIRQHandler(1);
generateSimpleIRQHandler(2);
generateSimpleIRQHandler(3);
generateSimpleIRQHandler(4);
generateComplexIRQHandler(5, 9);
generateComplexIRQHandler(10, 15);

#endif
