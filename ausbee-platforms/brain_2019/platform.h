#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAJOR                19
#define PLATFORM_MINOR                0

#define PLATFORM_LED0                 (1 << 0)

#define PLATFORM_GPIO_STARTER         (1 << 0)
#define PLATFORM_GPIO_EEPROM_WP       (1 << 0)

#define PLATFORM_USART_DEBUG          MCUAL_USART1

#define PLATFORM_ADC_VBAT             MCUAL_ADC10

#define PLATFORM_SPI_CS_UNSELECT      0
#define PLATFORM_SPI_GYRO_SELECT      1

#define PLATFORM_I2C_EEPROM           MCUAL_I2C1

#define PLATFORM_FLASH_SECTOR_BOOTLOADER_START  0
#define PLATFORM_FLASH_SECTOR_BOOTLOADER_END    0
#define PLATFORM_FLASH_SECTOR_PGM_START         1
#define PLATFORM_FLASH_SECTOR_PGM_END           11
#define PLATFORM_FLASH_PGM_START                0x08004000

void platform_init(void);
void platform_led_toggle(uint8_t led);
void platform_led_set(uint8_t led);
void platform_led_clear(uint8_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

int32_t platform_adc_get_mV(uint32_t adc);

void platform_spi_select(uint8_t select);
uint8_t platform_spi_transfert(uint8_t data);

#endif// PLATFORM_H
