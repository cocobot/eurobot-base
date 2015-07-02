#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_GPIO

#include <stdlib.h>
#include <mcual.h>
#include <avr/io.h>

static PORT_t * mcual_gpio_get_register(mcual_gpio_port_t port)
{
  switch(port)
  {
    case MCUAL_GPIOA:
      return &PORTA;

    case MCUAL_GPIOB:
      return &PORTB;

    case MCUAL_GPIOC:
      return &PORTC;

    case MCUAL_GPIOD:
      return &PORTD;

    case MCUAL_GPIOE:
      return &PORTE;

    case MCUAL_GPIOF:
      return &PORTF;

    case MCUAL_GPIOH:
      return &PORTH;

    case MCUAL_GPIOJ:
      return &PORTJ;

    case MCUAL_GPIOK:
      return &PORTK;
  }

  return NULL;
}

void mcual_gpio_init(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_direction_t direction)
{
  PORT_t * reg = mcual_gpio_get_register(port);
  
  if(direction == MCUAL_GPIO_INPUT)
  {
    reg->DIRCLR = pin;
  }
  else
  {
    reg->DIRSET = pin;
  }
}

void mcual_gpio_set(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  PORT_t * reg = mcual_gpio_get_register(port);

  reg->OUTSET = pin;
}

void mcual_gpio_clear(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  PORT_t * reg = mcual_gpio_get_register(port);

  reg->OUTCLR = pin;
}

void mcual_gpio_toogle(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  PORT_t * reg = mcual_gpio_get_register(port);

  reg->OUTTGL = pin;
}

uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  PORT_t * reg = mcual_gpio_get_register(port);

  return reg->IN & pin;
}


#endif

