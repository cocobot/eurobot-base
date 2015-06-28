#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#define PLATFORM_LED0 (1 << 0)
#define PLATFORM_LED1 (1 << 1)
#define PLATFORM_LED2 (1 << 2)

void platform_init(void);
void platform_led_toggle(uint8_t led);
void platform_led_set(uint8_t led);
void platform_led_clear(uint8_t led);

#endif// PLATFORM_H
