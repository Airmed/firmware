
#ifndef __BOARD_H__
#define __BOARD_H__

#define BOARD_PIN_OFF (0)
#define BOARD_PIN_ON (1)


typedef enum
{
    BOARD_SENSOR_HIGH_ADC = 0,
    BOARD_SENSOR_LOW_ADC,

    BOARD_ADC_COUNT
} board_adc_name;


typedef enum
{
    /* pins with callbacks first */
    BOARD_GPIO_BUTTON_PATIENT = 0,
    BOARD_GPIO_BUTTON_RESET,

    /* pins without callbacks */
    BOARD_GPIO_LED_STATUS,
    BOARD_GPIO_LED_ERROR,
    BOARD_GPIO_LED_CONNECTION,
    BOARD_SENSOR_HIGH,
    BOARD_SENSOR_LOW,
    BOARD_BUZZER,
    BOARD_SHUTTER_EN,
    BOARD_SHUTTER_SLEEP,
    BOARD_SHUTTER_RESET,
    BOARD_SHUTTER_DIR,
    BOARD_SHUTTER_STEP,
    BOARD_DRUM_EN,
    BOARD_DRUM_SLEEP,
    BOARD_DRUM_RESET,
    BOARD_DRUM_DIR,
    BOARD_DRUM_STEP
} board_gpio_name;


typedef enum
{
    BOARD_BUZZER_PWM = 0,

    BOARD_PWM_COUNT
} board_pwm_name;


typedef enum
{
    BOARD_SPI0 = 0,
    BOARD_SPI1,

    BOARD_SPI_COUNT
} board_spi_name;


typedef enum
{
    BOARD_TIMER0 = 0,
    BOARD_TIMER1,
    BOARD_TIMER2,

    BOARD_TIMER_COUNT
} board_timer_name;


typedef enum
{
    BOARD_UART0 = 0,
    BOARD_UART1,

    BOARD_UART_COUNT
} board_uart_name;


void hardware_init();
void software_init();

#endif
