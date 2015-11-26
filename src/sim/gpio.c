#include <mcual.h>


void mcual_gpio_init(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_direction_t direction)
{
  (void)port;
  (void)pin;
  (void)direction;
}

void mcual_gpio_set(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  (void)port;
  (void)pin;
}

void mcual_gpio_clear(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  (void)port;
  (void)pin;
}

void mcual_gpio_toogle(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  (void)port;
  (void)pin;
}

uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin)
{
  (void)port;
  (void)pin;

  return 0;
}

void mcual_gpio_set_function(mcual_gpio_port_t port, mcual_gpio_pin_t pin, int function_id)
{
  (void)port;
  (void)pin;
  (void)function_id;
}
