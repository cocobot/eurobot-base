#ifndef PLATFORM_H
#define PLATFORM_H

#include "generated/autoconf.h"
#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAJOR                19
#define PLATFORM_MINOR                0

#define PLATFORM_LED0                 (1 << 0)

#define PLATFORM_GPIO_UEN             (1 << 0)
#define PLATFORM_GPIO_VEN             (1 << 1)
#define PLATFORM_GPIO_WEN             (1 << 2)
#define PLATFORM_GPIO_UHALL           (1 << 3)
#define PLATFORM_GPIO_VHALL           (1 << 4)
#define PLATFORM_GPIO_WHALL           (1 << 5)

#define PLATFORM_PWM_U                (1 << 0)
#define PLATFORM_PWM_V                (1 << 1)
#define PLATFORM_PWM_W                (1 << 2)


#define PLATFORM_FLASH_SECTOR_BOOTLOADER_START  0
#define PLATFORM_FLASH_SECTOR_BOOTLOADER_END    7
#define PLATFORM_FLASH_SECTOR_PGM_START         8
#define PLATFORM_FLASH_SECTOR_PGM_END           255
#define PLATFORM_FLASH_PGM_START                0x08000000

#define PLATFORM_USART_USER           MCUAL_USART3

void platform_init(void);
void platform_led_toggle(uint32_t led);
void platform_led_set(uint32_t led);
void platform_led_clear(uint32_t led);

void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);
void platform_gpio_set_interrupt(uint32_t gpio, mcual_gpio_edge_t edge, mcual_gpio_interrupt_handler_t handler);

void platform_set_frequency(uint32_t pwm_output, uint32_t freq_Hz);
void platform_set_duty_cycle(uint32_t pwm_output, uint32_t duty_cycle);

#endif// PLATFORM_H
