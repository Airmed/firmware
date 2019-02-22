
#ifndef __BOARD_H__
#define __BOARD_H__

/* original board.h */

/* These #defines allow us to reuse TI-RTOS across other device families */

#define Board_ADC0                   CC3220S_LAUNCHXL_ADC0
#define Board_ADC1                   CC3220S_LAUNCHXL_ADC1

#define Board_CAPTURE0               CC3220S_LAUNCHXL_CAPTURE0
#define Board_CAPTURE1               CC3220S_LAUNCHXL_CAPTURE1

#define Board_CRYPTO0                CC3220S_LAUNCHXL_CRYPTO0

#define Board_GPIO_LED_ON            CC3220S_LAUNCHXL_GPIO_LED_ON
#define Board_GPIO_LED_OFF           CC3220S_LAUNCHXL_GPIO_LED_OFF

#define BOARD_PIN_OFF (0)
#define BOARD_PIN_ON (1)
/*
 *  CC3220S_LAUNCHXL_GPIO_LED_D8 and CC3220S_LAUNCHXL_GPIO_LED_D9 are shared with the I2C
 *  and PWM peripherals. In order for those examples to work, these LEDs are
 *  taken out of gpioPinCOnfig[]
 */
#define Board_GPIO_BUTTON0           CC3220S_LAUNCHXL_GPIO_SW2
#define Board_GPIO_BUTTON1           CC3220S_LAUNCHXL_GPIO_SW3

#define Board_GPIO_TMP116_EN         CC3220S_LAUNCHXL_GPIO_TMP116_EN

#define Board_I2C0                   CC3220S_LAUNCHXL_I2C0
#define Board_I2C_TMP                CC3220S_LAUNCHXL_I2C0

#define Board_I2S0                   CC3220S_LAUNCHXL_I2S0

#define Board_PWM0                   CC3220S_LAUNCHXL_PWM6
#define Board_PWM1                   CC3220S_LAUNCHXL_PWM7

#define Board_SD0                    CC3220S_LAUNCHXL_SD0

#define Board_SDFatFS0               CC3220S_LAUNCHXL_SD0

/* CC3220S_LAUNCHXL_SPI0 is reserved for the NWP */
#define Board_SPI0                   CC3220S_LAUNCHXL_SPI1

#define Board_SPI_MASTER             CC3220S_LAUNCHXL_SPI1
#define Board_SPI_SLAVE              CC3220S_LAUNCHXL_SPI1
#define Board_SPI_MASTER_READY       CC3220S_LAUNCHXL_SPI_MASTER_READY
#define Board_SPI_SLAVE_READY        CC3220S_LAUNCHXL_SPI_SLAVE_READY

#define Board_TIMER0                 CC3220S_LAUNCHXL_TIMER0
#define Board_TIMER1                 CC3220S_LAUNCHXL_TIMER1
#define Board_TIMER2                 CC3220S_LAUNCHXL_TIMER2

#define Board_UART0                  CC3220S_LAUNCHXL_UART0
#define Board_UART1                  CC3220S_LAUNCHXL_UART1

#define Board_WATCHDOG0              CC3220S_LAUNCHXL_WATCHDOG0

/* Board specific I2C address */
#define Board_TMP006_ADDR            (0x41)
#define Board_I2C_TMP006_ADDR        Board_TMP006_ADDR

/* CC3220S_LAUNCHXL.h */

#define CC3220S_LAUNCHXL_GPIO_LED_OFF (0)
#define CC3220S_LAUNCHXL_GPIO_LED_ON  (1)

/*!
 *  @def    CC3220S_LAUNCHXL_ADCName
 *  @brief  Enum of ADC names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_ADCName {
    CC3220S_LAUNCHXL_ADC0 = 0,
    CC3220S_LAUNCHXL_ADC1,

    CC3220S_LAUNCHXL_ADCCOUNT
} CC3220S_LAUNCHXL_ADCName;

/*!
 *  @def    CC3220S_LAUNCHXL_CaptureName
 *  @brief  Enum of Capture names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_CaptureName {
    CC3220S_LAUNCHXL_CAPTURE0 = 0,
    CC3220S_LAUNCHXL_CAPTURE1,

    CC3220S_LAUNCHXL_CAPTURECOUNT
} CC3220S_LAUNCHXL_CaptureName;

/*!
 *  @def    CC3220S_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_CryptoName {
    CC3220S_LAUNCHXL_CRYPTO0 = 0,

    CC3220S_LAUNCHXL_CRYPTOCOUNT
} CC3220S_LAUNCHXL_CryptoName;

typedef enum
{
    /* pins with callbacks first */
    BOARD_GPIO_BUTTON_PATIENT = 0,

    /* pins without callbacks */
    BOARD_GPIO_LED_STATUS,
    BOARD_GPIO_LED_ERROR,
    BOARD_GPIO_LED_CONNECTION,
    BOARD_GPIO_BUTTON_RESET,
    BOARD_GPIO_SENSOR,
    BOARD_BUZZER,
    BOARD_SHUTTER_EN,
    BOARD_SHUTTER_COIL0_P,
    BOARD_SHUTTER_COIL0_N,
    BOARD_SHUTTER_COIL1_P,
    BOARD_SHUTTER_COIL1_N,
    BOARD_DRUM_EN,
    BOARD_DRUM_COIL0_P,
    BOARD_DRUM_COIL0_N,
    BOARD_DRUM_COIL1_P,
    BOARD_DRUM_COIL1_N
} board_gpio_name;

/*!
 *  @def    CC3220S_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_I2CName {
    CC3220S_LAUNCHXL_I2C0 = 0,

    CC3220S_LAUNCHXL_I2CCOUNT
} CC3220S_LAUNCHXL_I2CName;

/*!
 *  @def    CC3220S_LAUNCHXL_I2SName
 *  @brief  Enum of I2S names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_I2SName {
    CC3220S_LAUNCHXL_I2S0 = 0,

    CC3220S_LAUNCHXL_I2SCOUNT
} CC3220S_LAUNCHXL_I2SName;

/*!
 *  @def    CC3220S_LAUNCHXL_PWMName
 *  @brief  Enum of PWM names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_PWMName {
    CC3220S_LAUNCHXL_PWM6 = 0,
    CC3220S_LAUNCHXL_PWM7,

    CC3220S_LAUNCHXL_PWMCOUNT
} CC3220S_LAUNCHXL_PWMName;

/*!
 *  @def    CC3220S_LAUNCHXL_SDFatFSName
 *  @brief  Enum of SDFatFS names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SDFatFSName {
    CC3220S_LAUNCHXL_SDFatFS0 = 0,

    CC3220S_LAUNCHXL_SDFatFSCOUNT
} CC3220S_LAUNCHXL_SDFatFSName;

/*!
 *  @def    CC3220S_LAUNCHXL_SDName
 *  @brief  Enum of SD names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SDName {
    CC3220S_LAUNCHXL_SD0 = 0,

    CC3220S_LAUNCHXL_SDCOUNT
} CC3220S_LAUNCHXL_SDName;

/*!
 *  @def    CC3220S_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SPIName {
    CC3220S_LAUNCHXL_SPI0 = 0,
    CC3220S_LAUNCHXL_SPI1,

    CC3220S_LAUNCHXL_SPICOUNT
} CC3220S_LAUNCHXL_SPIName;

/*!
 *  @def    CC3220S_LAUNCHXL_TimerName
 *  @brief  Enum of Timer names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_TimerName {
    CC3220S_LAUNCHXL_TIMER0 = 0,
    CC3220S_LAUNCHXL_TIMER1,
    CC3220S_LAUNCHXL_TIMER2,

    CC3220S_LAUNCHXL_TIMERCOUNT
} CC3220S_LAUNCHXL_TimerName;

/*!
 *  @def    CC3220S_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_UARTName {
    CC3220S_LAUNCHXL_UART0 = 0,
    CC3220S_LAUNCHXL_UART1,

    CC3220S_LAUNCHXL_UARTCOUNT
} CC3220S_LAUNCHXL_UARTName;

/*!
 *  @def    CC3220S_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_WatchdogName {
    CC3220S_LAUNCHXL_WATCHDOG0 = 0,

    CC3220S_LAUNCHXL_WATCHDOGCOUNT
} CC3220S_LAUNCHXL_WatchdogName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
extern void board_init(void);

#endif
