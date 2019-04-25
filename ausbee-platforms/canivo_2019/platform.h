#ifndef PLATFORM_H
#define PLATFORM_H

#include "generated/autoconf.h"
#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAJOR                19
#define PLATFORM_MINOR                0

#ifdef CONFIG_PLATFORM_CANIVO_2019_UART
# define PLATFORM_USART_USER          MCUAL_USART2
#endif

void platform_init(void);
void platform_led_toggle(uint32_t led);
void platform_led_set(uint32_t led);
void platform_led_clear(uint32_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

#endif// PLATFORM_H
