#ifndef PLATFORM_H
#define PLATFORM_H

#include <mcual.h>
#include <stdint.h>

#define PLATFORM_LED0 (1 << 0)
#define PLATFORM_LED1 (1 << 1)
#define PLATFORM_LED2 (1 << 2)

#define PLATFORM_GPIO0                        (1 << 0)
#define PLATFORM_GPIO1                        (1 << 1)
#define PLATFORM_GPIO2                        (1 << 2)
#define PLATFORM_GPIO3                        (1 << 3)
#define PLATFORM_GPIO4                        (1 << 4)
#define PLATFORM_GPIO5                        (1 << 5)
#define PLATFORM_GPIO6                        (1 << 6)
#define PLATFORM_GPIO7                        (1 << 7)
#define PLATFORM_GPIO8                        (1 << 8)
#define PLATFORM_GPIO9                        (1 << 9)
#define PLATFORM_GPIO10                       (1 << 10)
#define PLATFORM_GPIO11                       (1 << 11)
#define PLATFORM_GPIO_STARTER                 (1 << 12)
#define PLATFORM_GPIO_COLOR                   (1 << 13)
#define PLATFORM_GPIO_ALARM0                  (1 << 14)
#define PLATFORM_GPIO_ALARM1                  (1 << 15)
#define PLATFORM_GPIO_ALARM2                  (1 << 16)
#define PLATFORM_GPIO_ALARM3                  (1 << 17)
#define PLATFORM_GPIO_MOTOR_DIR_RIGHT         (1 << 18)
#define PLATFORM_GPIO_MOTOR_DIR_LEFT          (1 << 19)
#define PLATFORM_GPIO_MOTOR_ENABLE            (1 << 20)

#define PLATFORM_USART_DEBUG MCUAL_USART1

#define PLATFORM_ADC_VBAT   MCUAL_ADC0
#define PLATFORM_ADC_IR0    MCUAL_ADC1
#define PLATFORM_ADC_IR1    MCUAL_ADC2
#define PLATFORM_ADC_IR2    MCUAL_ADC3
#define PLATFORM_ADC_IR3    MCUAL_ADC4
#define PLATFORM_ADC_CH0    MCUAL_ADC5
#define PLATFORM_ADC_CH1    MCUAL_ADC6
#define PLATFORM_ADC_CH2    MCUAL_ADC7
#define PLATFORM_ADC_CH3    MCUAL_ADC16
#define PLATFORM_ADC_CH4    MCUAL_ADC17

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

#define PLATFORM_I2C_SERVO            MCUAL_I2C1

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
#define PLATFORM_SERVO_13_ID             13
#define PLATFORM_SERVO_14_ID             14
#define PLATFORM_SERVO_15_ID             15
#define PLATFORM_SERVO_16_ID             16

#define PLATFORM_US_0                     0
#define PLATFORM_US_1                     1
#define PLATFORM_US_2                     2
#define PLATFORM_US_3                     3

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

void platform_spi_position_select(uint8_t select);
uint8_t platform_spi_position_transfert(uint8_t data);

void platform_spi_slave_select(uint8_t select);
uint8_t platform_spi_slave_transfert(uint8_t data);

void platform_motor_set_frequency(uint32_t freq_Hz);
void platform_motor_set_left_duty_cycle(uint32_t duty_cycle);
void platform_motor_set_right_duty_cycle(uint32_t duty_cycle);

void platform_servo_set_value(uint32_t servo_id, uint32_t value);

uint8_t platform_i2c_transmit(mcual_i2c_id_t id, uint8_t addr, uint8_t * txbuf, uint8_t tx_size, uint8_t * rxbuf, uint8_t rx_size);

void platform_us_send_trig(uint32_t us_id);
void platform_us_reset_trig(uint32_t us_id);
float platform_us_get_value(uint32_t us_id);

#endif// PLATFORM_H
