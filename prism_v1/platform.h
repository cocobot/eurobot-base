#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_LED0         (1 << 0)
#define PLATFORM_LED1         (1 << 1)
#define PLATFORM_LED2         (1 << 2)
#define PLATFORM_LED3         (1 << 3)
#define PLATFORM_LED4         (1 << 4)
#define PLATFORM_LED5         (1 << 5)
#define PLATFORM_LED_ERROR    (1 << 6)
#define PLATFORM_LED_RUN      (1 << 7)
#define PLATFORM_LED_COM      (1 << 8)
#define PLATFORM_LED_SYNC0    (1 << 9)
#define PLATFORM_LED_SYNC1    (1 << 10)
#define PLATFORM_LED_LOWBAT   (1 << 11)
#define PLATFORM_LED_IDX0     (1 << 12)
#define PLATFORM_LED_IDX1     (1 << 13)
#define PLATFORM_LED_RF       (1 << 14)

#define PLATFORM_ADC_VBAT     (1 << 0)

#define PLATFORM_USART_XBEE   MCUAL_USART1
#define PLATFORM_USART_BAL1   MCUAL_USART6
#define PLATFORM_USART_BAL2   MCUAL_USART7

void platform_init(void);
void platform_led_toggle(uint32_t led);
void platform_led_set(uint32_t led);
void platform_led_clear(uint32_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toogle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

#endif// PLATFORM_H
