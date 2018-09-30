#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_LED_GREEN  (1 << 0)
#define PLATFORM_LED_ORANGE (1 << 1)
#define PLATFORM_LED_RED    (1 << 2)
#define PLATFORM_LED_BLUE   (1 << 3)

#define PLATFORM_BUTTON_USER                  (1 << 0)

#define PLATFORM_USART_DEBUG MCUAL_USART1

#define PLATFORM_SPI_CS_UNSELECT      0
#define PLATFORM_SPI_CS0_SELECT       1
#define PLATFORM_SPI_CS1_SELECT       2
#define PLATFORM_SPI_CS2_SELECT       3
#define PLATFORM_SPI_CS3_SELECT       4
#define PLATFORM_SPI_CS4_SELECT       5
#define PLATFORM_SPI_CS5_SELECT       6
#define PLATFORM_SPI_CS6_SELECT       7
#define PLATFORM_SPI_CS7_SELECT       8
#define PLATFORM_SPI_CS8_SELECT       9
#define PLATFORM_SPI_CS9_SELECT      10
#define PLATFORM_SPI_CS10_SELECT     11
#define PLATFORM_SPI_CS11_SELECT     12
#define PLATFORM_SPI_ENCR_SELECT      1
#define PLATFORM_SPI_ENCL_SELECT      2
#define PLATFORM_SPI_ENCG_SELECT      3

#define PLATFORM_I2C_VL53L0X          MCUAL_I2C1

void platform_init(void);
void platform_led_toggle(uint8_t led);
void platform_led_set(uint8_t led);
void platform_led_clear(uint8_t led);

void platform_gpio_set_direction(uint32_t gpio, mcual_gpio_direction_t direction);
void platform_gpio_set(uint32_t gpio);
void platform_gpio_clear(uint32_t gpio);
void platform_gpio_toggle(uint32_t gpio);
uint32_t platform_gpio_get(uint32_t gpio);

uint8_t platform_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size);

#endif// PLATFORM_H
