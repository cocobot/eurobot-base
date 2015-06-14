#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#define PLATFORM_LED0 (1 << 0)
#define PLATFORM_LED1 (1 << 1)

void platform_init(void);
void platform_led_toggle(uint8_t led);

#endif// PLATFORM_H
