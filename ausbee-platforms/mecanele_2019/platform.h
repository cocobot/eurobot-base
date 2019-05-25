#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_MAJOR                19
#define PLATFORM_MINOR                0

#define PLATFORM_USART_DEBUG          MCUAL_USART1
#define PLATFORM_USART_USER           MCUAL_USART1

#define PLATFORM_LED0                 (1 << 0)

#define PLATFORM_GPIO_PUMP1           (1 << 0)
#define PLATFORM_GPIO_PUMP2           (1 << 1)
#define PLATFORM_GPIO_VALVE1          (1 << 2)
#define PLATFORM_GPIO_VALVE2          (1 << 3)
#define PLATFORM_GPIO_VALVE3          (1 << 4)
#define PLATFORM_GPIO_VALVE4          (1 << 5)
#define PLATFORM_GPIO_VALVE5          (1 << 7)
#define PLATFORM_GPIO_VALVE6          (1 << 8)
#define PLATFORM_GPIO_VALVE7          (1 << 9)
#define PLATFORM_GPIO_VALVE8          (1 << 10)

#define PLATFORM_ADC_ARM_1             MCUAL_ADC6
#define PLATFORM_ADC_ARM_2             MCUAL_ADC7
#define PLATFORM_ADC_ARM_3             MCUAL_ADC4
#define PLATFORM_ADC_ARM_4             MCUAL_ADC5

#define PLATFORM_SERVO_0_ID               0
#define PLATFORM_SERVO_1_ID               1
#define PLATFORM_SERVO_2_ID               2
#define PLATFORM_SERVO_3_ID               3
#define PLATFORM_SERVO_4_ID               4
#define PLATFORM_SERVO_5_ID               5
#define PLATFORM_SERVO_6_ID               6
#define PLATFORM_SERVO_7_ID               7
#define PLATFORM_SERVO_8_ID               8
#define PLATFORM_SERVO_9_ID               9
#define PLATFORM_SERVO_10_ID             10
#define PLATFORM_SERVO_11_ID             11
#define PLATFORM_SERVO_12_ID             12

#define PLATFORM_I2C_SERVO              MCUAL_I2C1

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

uint8_t platform_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size);
void platform_servo_set_value(uint32_t servo_id, uint32_t value);

void uprintf(char * fmt, ...);

#endif// PLATFORM_H
