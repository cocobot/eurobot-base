#ifndef MCUAL_GPIO_H
#define MCUAL_GPIO_H

#include <stdint.h>

typedef uint32_t mcual_gpio_port_t;
typedef uint32_t mcual_gpio_pin_t;

#define MCUAL_GPIOA ((mcual_gpio_port_t) 0)
#define MCUAL_GPIOB ((mcual_gpio_port_t) 1)
#define MCUAL_GPIOC ((mcual_gpio_port_t) 2)
#define MCUAL_GPIOD ((mcual_gpio_port_t) 3)
#define MCUAL_GPIOE ((mcual_gpio_port_t) 4)
#define MCUAL_GPIOF ((mcual_gpio_port_t) 5)
#define MCUAL_GPIOG ((mcual_gpio_port_t) 6)
#define MCUAL_GPIOH ((mcual_gpio_port_t) 7)
#define MCUAL_GPIOI ((mcual_gpio_port_t) 8)
#define MCUAL_GPIOJ ((mcual_gpio_port_t) 9)
#define MCUAL_GPIOK ((mcual_gpio_port_t) 10)

#define MCUAL_GPIO_PIN0   (((mcual_gpio_pin_t)1) << 0)
#define MCUAL_GPIO_PIN1   (((mcual_gpio_pin_t)1) << 1)
#define MCUAL_GPIO_PIN2   (((mcual_gpio_pin_t)1) << 2)
#define MCUAL_GPIO_PIN3   (((mcual_gpio_pin_t)1) << 3)
#define MCUAL_GPIO_PIN4   (((mcual_gpio_pin_t)1) << 4)
#define MCUAL_GPIO_PIN5   (((mcual_gpio_pin_t)1) << 5)
#define MCUAL_GPIO_PIN6   (((mcual_gpio_pin_t)1) << 6)
#define MCUAL_GPIO_PIN7   (((mcual_gpio_pin_t)1) << 7)
#define MCUAL_GPIO_PIN8   (((mcual_gpio_pin_t)1) << 8)
#define MCUAL_GPIO_PIN9   (((mcual_gpio_pin_t)1) << 9)
#define MCUAL_GPIO_PIN10  (((mcual_gpio_pin_t)1) << 10)
#define MCUAL_GPIO_PIN11  (((mcual_gpio_pin_t)1) << 11)
#define MCUAL_GPIO_PIN12  (((mcual_gpio_pin_t)1) << 12)
#define MCUAL_GPIO_PIN13  (((mcual_gpio_pin_t)1) << 13)
#define MCUAL_GPIO_PIN14  (((mcual_gpio_pin_t)1) << 14)
#define MCUAL_GPIO_PIN15  (((mcual_gpio_pin_t)1) << 15)

#define MCUAL_GPIO_FUNCTION_ANALOG (-1)

typedef enum
{
  MCUAL_GPIO_INPUT,
  MCUAL_GPIO_OUTPUT,
} mcual_gpio_direction_t;

typedef enum
{
  MCUAL_GPIO_NO_RESISTOR,
  MCUAL_GPIO_PULL_UP_RESISTOR,
  MCUAL_GPIO_PULL_DOWN_RESISTOR,
} mcual_gpio_pull_resistor_t;


void mcual_gpio_init(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_direction_t direction);
void mcual_gpio_set_pull_resistor(mcual_gpio_port_t port, mcual_gpio_pin_t pin, mcual_gpio_pull_resistor_t pull);

void mcual_gpio_set(mcual_gpio_port_t port, mcual_gpio_pin_t pin);
void mcual_gpio_clear(mcual_gpio_port_t port, mcual_gpio_pin_t pin);
void mcual_gpio_toogle(mcual_gpio_port_t port, mcual_gpio_pin_t pin);
uint32_t mcual_gpio_get(mcual_gpio_port_t port, mcual_gpio_pin_t pin);

void mcual_gpio_set_function(mcual_gpio_port_t port, mcual_gpio_pin_t pin, int function_id);

#endif // MCUAL_GPIO_H
