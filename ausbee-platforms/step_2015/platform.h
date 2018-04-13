#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAIN_CLOCK_KHZ 32000

#define PLATFORM_LED0 (1 << 0)

#define PLATFORM_GPIO0                        (1 << 0)
#define PLATFORM_GPIO1                        (1 << 1)
#define PLATFORM_GPIO2                        (1 << 2)
#define PLATFORM_GPIO3                        (1 << 3)

void platform_init(void);
void platform_led_toggle(uint8_t led);
void platform_led_set(uint8_t led);
void platform_led_clear(uint8_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

#endif// PLATFORM_H
