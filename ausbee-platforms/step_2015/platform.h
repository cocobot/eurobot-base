#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAIN_CLOCK_KHZ 32000

#define PLATFORM_LED0 (1 << 0)

#define PLATFORM_GPIO0                        (1UL << 0)
#define PLATFORM_GPIO1                        (1UL << 1)
#define PLATFORM_GPIO2                        (1UL << 2)
#define PLATFORM_GPIO3                        (1UL << 3)
#define PLATFORM_STEPPER_MS1                  (1UL << 4)
#define PLATFORM_STEPPER_MS2                  (1UL << 5)
#define PLATFORM_STEPPER_MS3                  (1UL << 6)
#define PLATFORM_STEPPER_RESET                (1UL << 7)
#define PLATFORM_STEPPER_SLEEP                (1UL << 8)
#define PLATFORM_STEPPER_ENABLE               (1UL << 9)
#define PLATFORM_STEPPER_M0_DIR               (1UL << 10)
#define PLATFORM_STEPPER_M0_STEP              (1UL << 11)
#define PLATFORM_STEPPER_M1_DIR               (1UL << 12)
#define PLATFORM_STEPPER_M1_STEP              (1UL << 13)
#define PLATFORM_STEPPER_M2_DIR               (1UL << 14)
#define PLATFORM_STEPPER_M2_STEP              (1UL << 15)
#define PLATFORM_STEPPER_M3_DIR               (1UL << 16)
#define PLATFORM_STEPPER_M3_STEP              (1UL << 17)

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
