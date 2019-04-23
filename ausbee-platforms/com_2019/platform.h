#ifndef PLATFORM_H
#define PLATFORM_H

#include "generated/autoconf.h"
#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAJOR                19
#define PLATFORM_MINOR                0

#define PLATFORM_LED_RED_0            (1 << 0)
#define PLATFORM_LED_RED_1            (1 << 1)
#define PLATFORM_LED_RED_2            (1 << 2)
#define PLATFORM_LED_RED_3            (1 << 3)
#define PLATFORM_LED_RED_4            (1 << 4)
#define PLATFORM_LED_RED_5            (1 << 5)
#define PLATFORM_LED_RED_6            (1 << 6)
#define PLATFORM_LED_RED_7            (1 << 7)
#define PLATFORM_LED_RED_8            (1 << 8)
#define PLATFORM_LED_RED_9            (1 << 9)
#define PLATFORM_LED_GREEN_0          (1 << 10)
#define PLATFORM_LED_GREEN_1          (1 << 11)
#define PLATFORM_LED_GREEN_2          (1 << 12)
#define PLATFORM_LED_GREEN_3          (1 << 13)
#define PLATFORM_LED_GREEN_4          (1 << 14)
#define PLATFORM_LED_GREEN_5          (1 << 15)
#define PLATFORM_LED_GREEN_6          (1 << 16)
#define PLATFORM_LED_GREEN_7          (1 << 17)
#define PLATFORM_LED_GREEN_8          (1 << 18)
#define PLATFORM_LED_GREEN_9          (1 << 19)

#define PLATFORM_GPIO_SW_0            (1 << 0)
#define PLATFORM_GPIO_SW_1            (1 << 1)
#define PLATFORM_GPIO_SW_2            (1 << 2)
#define PLATFORM_GPIO_SW_3            (1 << 3)
#define PLATFORM_GPIO_SW_4            (1 << 4)
#define PLATFORM_GPIO_SW_5            (1 << 5)
#define PLATFORM_GPIO_SW_6            (1 << 6)
#define PLATFORM_GPIO_SW_7            (1 << 7)
#define PLATFORM_GPIO_RF_0            (1 << 8)
#define PLATFORM_GPIO_RF_1            (1 << 9)
#define PLATFORM_GPIO_RF_2            (1 << 10)
#define PLATFORM_GPIO_RF_3            (1 << 11)
#define PLATFORM_GPIO_SDN             (1 << 12)

#define PLATFORM_USART_DEBUG          MCUAL_USART1
#ifdef CONFIG_PLATFORM_COM_2019_USB
# define PLATFORM_USART_USER          MCUAL_USART2
#endif

#define PLATFORM_SPI_CS_UNSELECT      0
#define PLATFORM_SPI_RF_SELECT        1

#define PLATFORM_I2C_EEPROM           MCUAL_I2C1

#define PLATFORM_FLASH_SECTOR_BOOTLOADER_START  0
#define PLATFORM_FLASH_SECTOR_BOOTLOADER_END    0
#define PLATFORM_FLASH_SECTOR_PGM_START         1
#define PLATFORM_FLASH_SECTOR_PGM_END           11
#define PLATFORM_FLASH_PGM_START                0x08004000

void platform_init(void);
void platform_led_toggle(uint32_t led);
void platform_led_set(uint32_t led);
void platform_led_clear(uint32_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

void platform_spi_select(uint8_t select);
uint8_t platform_spi_transfert(uint8_t data);
void uprintf(char * fmt, ...);

#endif// PLATFORM_H
