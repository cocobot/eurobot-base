#include <stdlib.h>
#include <stm32f4xx.h>
#include <mcual.h>

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
        moder |= (0x01 << (2 * i));
      }

      //set high speed
      ospeedr |= (0x03 << (2 * i));

      //set function 0 (GPIO)
      if(i < 8)
      {
        afrl &= ~(0x03 << (4 * i));
      }
      else
      {
        afrh &= ~(0x03 << (4 * (i - 8)));
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

void mcual_gpio_toogle(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  reg->ODR ^= pin;
}

uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  GPIO_TypeDef * reg = mcual_gpio_get_register(port);

  return reg->IDR & pin;
}
