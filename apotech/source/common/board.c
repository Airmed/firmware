
#include "board.h"

#include "buzzer.h"
#include "drum.h"
#include "flash.h"
#include "leds.h"
#include "pin_map.h"
#include "sensor.h"
#include "shutter.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_types.h>

#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/adc.h>
#include <ti/devices/cc32xx/driverlib/gpio.h>
#include <ti/devices/cc32xx/driverlib/pin.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/spi.h>
#include <ti/devices/cc32xx/driverlib/sdhost.h>
#include <ti/devices/cc32xx/driverlib/timer.h>
#include <ti/devices/cc32xx/driverlib/uart.h>
#include <ti/devices/cc32xx/driverlib/udma.h>
#include <ti/devices/cc32xx/driverlib/wdt.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC32XX.h>
#include <ti/drivers/dma/UDMACC32XX.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC32XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerCC32XX.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC32XXDMA.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerCC32XX.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC32XX.h>

#include <ti/drivers/net/wifi/simplelink.h>

/*
 *  =============================== ADC ===============================
 */

ADCCC32XX_Object adcCC3220SObjects[BOARD_ADC_COUNT];

const ADCCC32XX_HWAttrsV1 adcCC3220SHWAttrs[BOARD_ADC_COUNT] = {
    {
        .adcPin = BOARD_SENSOR_HIGH_ADC_PIN
    },
    {
        .adcPin = BOARD_SENSOR_LOW_ADC_PIN
    }
};

const ADC_Config ADC_config[BOARD_ADC_COUNT] = {
    {
        .fxnTablePtr = &ADCCC32XX_fxnTable,
        .object = &adcCC3220SObjects[BOARD_SENSOR_HIGH_ADC],
        .hwAttrs = &adcCC3220SHWAttrs[BOARD_SENSOR_HIGH_ADC]
    },
    {
        .fxnTablePtr = &ADCCC32XX_fxnTable,
        .object = &adcCC3220SObjects[BOARD_SENSOR_LOW_ADC],
        .hwAttrs = &adcCC3220SHWAttrs[BOARD_SENSOR_LOW_ADC]
    }
};

const uint_least8_t ADC_count = BOARD_ADC_COUNT;

/*
 *  =============================== DMA ===============================
 */

static tDMAControlTable dmaControlTable[64] __attribute__ ((aligned (1024)));

/*
 *  ======== dmaErrorFxn ========
 *  This is the handler for the uDMA error interrupt.
 */
static void dmaErrorFxn(uintptr_t arg)
{
    int status = MAP_uDMAErrorStatusGet();
    MAP_uDMAErrorStatusClear();

    /* Suppress unused variable warning */
    (void)status;

    while (1);
}

UDMACC32XX_Object udmaCC3220SObject;

const UDMACC32XX_HWAttrs udmaCC3220SHWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMACC32XX_ErrorFxn)dmaErrorFxn,
    .intNum = INT_UDMAERR,
    .intPriority = (~0)
};

const UDMACC32XX_Config UDMACC32XX_config = {
    .object = &udmaCC3220SObject,
    .hwAttrs = &udmaCC3220SHWAttrs
};

/*
 *  =============================== GPIO ===============================
 */

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in CC3220S_LAUNCHXL.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* pins with callbacks first */
    BOARD_GPIO_BUTTON_PATIENT_PIN | GPIO_CFG_INPUT | GPIO_CFG_IN_INT_RISING,
    BOARD_GPIO_BUTTON_RESET_PIN | GPIO_CFG_INPUT,

    /* pins without callbacks */
    BOARD_GPIO_LED_STATUS_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_GPIO_LED_ERROR_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_GPIO_LED_CONNECTION_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SENSOR_HIGH_PIN | GPIO_CFG_INPUT,
    BOARD_SENSOR_LOW_PIN | GPIO_CFG_INPUT,
    BOARD_BUZZER_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SHUTTER_EN_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SHUTTER_SLEEP_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SHUTTER_RESET_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SHUTTER_DIR_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_SHUTTER_STEP_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_DRUM_EN_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_DRUM_SLEEP_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_DRUM_RESET_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_DRUM_DIR_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    BOARD_DRUM_STEP_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in CC3220S_LAUNCHXL.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,  /* CC3220S_LAUNCHXL_GPIO_SW2 */
};

/* The device-specific GPIO_config structure */
const GPIOCC32XX_Config GPIOCC32XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  =============================== Power ===============================
 */
/*
 * This table defines the parking state to be set for each parkable pin
 * during LPDS. (Device pins must be parked during LPDS to achieve maximum
 * power savings.)  If the pin should be left unparked, specify the state
 * PowerCC32XX_DONT_PARK.  For example, for a UART TX pin, the device
 * will automatically park the pin in a high state during transition to LPDS,
 * so the Power Manager does not need to explictly park the pin.  So the
 * corresponding entries in this table should indicate PowerCC32XX_DONT_PARK.
 */
PowerCC32XX_ParkInfo parkInfo[] = {
/*          PIN                    PARK STATE              PIN ALIAS (FUNCTION)
     -----------------  ------------------------------     -------------------- */
    {PowerCC32XX_PIN01, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO10              */
    {PowerCC32XX_PIN02, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO11              */
    {PowerCC32XX_PIN03, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO12              */
    {PowerCC32XX_PIN04, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO13              */
    {PowerCC32XX_PIN05, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO14              */
    {PowerCC32XX_PIN06, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO15              */
    {PowerCC32XX_PIN07, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO16              */
    {PowerCC32XX_PIN08, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO17              */
    {PowerCC32XX_PIN13, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* FLASH_SPI_DIN       */
    {PowerCC32XX_PIN15, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO22              */
    {PowerCC32XX_PIN16, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* TDI (JTAG DEBUG)    */
    {PowerCC32XX_PIN17, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* TDO (JTAG DEBUG)    */
    {PowerCC32XX_PIN19, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* TCK (JTAG DEBUG)    */
    {PowerCC32XX_PIN20, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* TMS (JTAG DEBUG)    */
    {PowerCC32XX_PIN18, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO28              */
    {PowerCC32XX_PIN21, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* SOP2                */
    {PowerCC32XX_PIN29, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* ANTSEL1             */
    {PowerCC32XX_PIN30, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* ANTSEL2             */
    {PowerCC32XX_PIN45, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* DCDC_ANA2_SW_P      */
    {PowerCC32XX_PIN50, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO0               */
    {PowerCC32XX_PIN52, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* RTC_XTAL_N          */
    {PowerCC32XX_PIN53, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO30              */
    {PowerCC32XX_PIN55, PowerCC32XX_WEAK_PULL_UP_STD},   /* GPIO1 (XDS_UART_RX) */
    {PowerCC32XX_PIN57, PowerCC32XX_WEAK_PULL_UP_STD},   /* GPIO2 (XDS_UART_TX) */
    {PowerCC32XX_PIN58, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO3               */
    {PowerCC32XX_PIN59, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO4               */
    {PowerCC32XX_PIN60, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO5               */
    {PowerCC32XX_PIN61, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO6               */
    {PowerCC32XX_PIN62, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO7               */
    {PowerCC32XX_PIN63, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO8               */
    {PowerCC32XX_PIN64, PowerCC32XX_WEAK_PULL_DOWN_STD}, /* GPIO9               */
};

/*
 *  This structure defines the configuration for the Power Manager.
 *
 *  In this configuration the Power policy is disabled by default (because
 *  enablePolicy is set to false).  The Power policy can be enabled dynamically
 *  at runtime by calling Power_enablePolicy(), or at build time, by changing
 *  enablePolicy to true in this structure.
 */
const PowerCC32XX_ConfigV1 PowerCC32XX_config = {
    .policyInitFxn = &PowerCC32XX_initPolicy,
    .policyFxn = &PowerCC32XX_sleepPolicy,
    .enterLPDSHookFxn = NULL,
    .resumeLPDSHookFxn = NULL,
    .enablePolicy = false,
    .enableGPIOWakeupLPDS = true,
    .enableGPIOWakeupShutdown = true,
    .enableNetworkWakeupLPDS = true,
    .wakeupGPIOSourceLPDS = PRCM_LPDS_GPIO13,
    .wakeupGPIOTypeLPDS = PRCM_LPDS_FALL_EDGE,
    .wakeupGPIOFxnLPDS = NULL,
    .wakeupGPIOFxnLPDSArg = 0,
    .wakeupGPIOSourceShutdown = PRCM_HIB_GPIO13,
    .wakeupGPIOTypeShutdown = PRCM_HIB_RISE_EDGE,
    .ramRetentionMaskLPDS = PRCM_SRAM_COL_1 | PRCM_SRAM_COL_2 |
        PRCM_SRAM_COL_3 | PRCM_SRAM_COL_4,
    .keepDebugActiveDuringLPDS = false,
    .ioRetentionShutdown = PRCM_IO_RET_GRP_1,
    .pinParkDefs = parkInfo,
    .numPins = sizeof(parkInfo) / sizeof(PowerCC32XX_ParkInfo)
};

/*
 *  =============================== PWM ===============================
 */

PWMTimerCC32XX_Object pwmTimerCC3220SObjects[BOARD_PWM_COUNT];

const PWMTimerCC32XX_HWAttrsV2 pwmTimerCC3220SHWAttrs[BOARD_PWM_COUNT] = {
    {    /* CC3220S_LAUNCHXL_PWM6 */
        .pwmPin = BOARD_BUZZER_PWM_PIN
    }
};

const PWM_Config PWM_config[BOARD_PWM_COUNT] = {
    {
        .fxnTablePtr = &PWMTimerCC32XX_fxnTable,
        .object = &pwmTimerCC3220SObjects[BOARD_BUZZER_PWM],
        .hwAttrs = &pwmTimerCC3220SHWAttrs[BOARD_BUZZER_PWM]
    }
};

const uint_least8_t PWM_count = BOARD_PWM_COUNT;


/*
 *  =============================== SPI ===============================
 */

SPICC32XXDMA_Object spiCC3220SDMAObjects[BOARD_SPI_COUNT];

uint32_t spiCC3220SDMAscratchBuf[BOARD_SPI_COUNT];

const SPICC32XXDMA_HWAttrsV1 spiCC3220SDMAHWAttrs[BOARD_SPI_COUNT] = {
    /* index 0 is reserved for LSPI that links to the NWP */
    {
        .baseAddr = LSPI_BASE,
        .intNum = INT_LSPI,
        .intPriority = (~0),
        .spiPRCM = PRCM_LSPI,
        .csControl = SPI_SW_CTRL_CS,
        .csPolarity = SPI_CS_ACTIVEHIGH,
        .pinMode = SPI_4PIN_MODE,
        .turboMode = SPI_TURBO_OFF,
        .scratchBufPtr = &spiCC3220SDMAscratchBuf[BOARD_SPI0],
        .defaultTxBufValue = 0,
        .rxChannelIndex = UDMA_CH12_LSPI_RX,
        .txChannelIndex = UDMA_CH13_LSPI_TX,
        .minDmaTransferSize = 100,
        .mosiPin = SPICC32XXDMA_PIN_NO_CONFIG,
        .misoPin = SPICC32XXDMA_PIN_NO_CONFIG,
        .clkPin = SPICC32XXDMA_PIN_NO_CONFIG,
        .csPin = SPICC32XXDMA_PIN_NO_CONFIG
    },
    {
        .baseAddr = GSPI_BASE,
        .intNum = INT_GSPI,
        .intPriority = (~0),
        .spiPRCM = PRCM_GSPI,
        .csControl = SPI_HW_CTRL_CS,
        .csPolarity = SPI_CS_ACTIVELOW,
        .pinMode = SPI_4PIN_MODE,
        .turboMode = SPI_TURBO_OFF,
        .scratchBufPtr = &spiCC3220SDMAscratchBuf[BOARD_SPI1],
        .defaultTxBufValue = 0,
        .rxChannelIndex = UDMA_CH6_GSPI_RX,
        .txChannelIndex = UDMA_CH7_GSPI_TX,
        .minDmaTransferSize = 10,
        .mosiPin = SPICC32XXDMA_PIN_07_MOSI,
        .misoPin = SPICC32XXDMA_PIN_06_MISO,
        .clkPin = SPICC32XXDMA_PIN_05_CLK,
        .csPin = SPICC32XXDMA_PIN_08_CS
    }
};

const SPI_Config SPI_config[BOARD_SPI_COUNT] = {
    {
        .fxnTablePtr = &SPICC32XXDMA_fxnTable,
        .object = &spiCC3220SDMAObjects[BOARD_SPI0],
        .hwAttrs = &spiCC3220SDMAHWAttrs[BOARD_SPI0]
    },
    {
        .fxnTablePtr = &SPICC32XXDMA_fxnTable,
        .object = &spiCC3220SDMAObjects[BOARD_SPI1],
        .hwAttrs = &spiCC3220SDMAHWAttrs[BOARD_SPI1]
    }
};

const uint_least8_t SPI_count = BOARD_SPI_COUNT;

/*
 *  =============================== Timer ===============================
 */

TimerCC32XX_Object timerCC3220SObjects[BOARD_TIMER_COUNT];

const TimerCC32XX_HWAttrs timerCC3220SHWAttrs[BOARD_TIMER_COUNT] = {
    {
        .baseAddress = TIMERA0_BASE,
        .subTimer = TimerCC32XX_timer32,
        .intNum = INT_TIMERA0A,
        .intPriority = ~0
    },
    {
        .baseAddress = TIMERA1_BASE,
        .subTimer = TimerCC32XX_timer16A,
        .intNum = INT_TIMERA1A,
        .intPriority = ~0
    },
    {
         .baseAddress = TIMERA1_BASE,
         .subTimer = TimerCC32XX_timer16B,
         .intNum = INT_TIMERA1B,
         .intPriority = ~0
    },
};

const Timer_Config Timer_config[BOARD_TIMER_COUNT] = {
    {
        .fxnTablePtr = &TimerCC32XX_fxnTable,
        .object = &timerCC3220SObjects[BOARD_TIMER0],
        .hwAttrs = &timerCC3220SHWAttrs[BOARD_TIMER0]
    },
    {
        .fxnTablePtr = &TimerCC32XX_fxnTable,
        .object = &timerCC3220SObjects[BOARD_TIMER1],
        .hwAttrs = &timerCC3220SHWAttrs[BOARD_TIMER1]
    },
    {
        .fxnTablePtr = &TimerCC32XX_fxnTable,
        .object = &timerCC3220SObjects[BOARD_TIMER2],
        .hwAttrs = &timerCC3220SHWAttrs[BOARD_TIMER2]
    },
};

const uint_least8_t Timer_count = BOARD_TIMER_COUNT;

/*
 *  =============================== UART ===============================
 */

UARTCC32XX_Object uartCC3220SObjects[BOARD_UART_COUNT];
unsigned char uartCC3220SRingBuffer[BOARD_UART_COUNT][32];

/* UART configuration structure */
const UARTCC32XX_HWAttrsV1 uartCC3220SHWAttrs[BOARD_UART_COUNT] = {
    {
        .baseAddr = UARTA0_BASE,
        .intNum = INT_UARTA0,
        .intPriority = (~0),
        .flowControl = UARTCC32XX_FLOWCTRL_NONE,
        .ringBufPtr  = uartCC3220SRingBuffer[BOARD_UART0],
        .ringBufSize = sizeof(uartCC3220SRingBuffer[BOARD_UART0]),
        .rxPin = UARTCC32XX_PIN_57_UART0_RX,
        .txPin = UARTCC32XX_PIN_55_UART0_TX,
        .ctsPin = UARTCC32XX_PIN_UNASSIGNED,
        .rtsPin = UARTCC32XX_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UARTA1_BASE,
        .intNum = INT_UARTA1,
        .intPriority = (~0),
        .flowControl = UARTCC32XX_FLOWCTRL_NONE,
        .ringBufPtr  = uartCC3220SRingBuffer[BOARD_UART1],
        .ringBufSize = sizeof(uartCC3220SRingBuffer[BOARD_UART1]),
        .rxPin = UARTCC32XX_PIN_08_UART1_RX,
        .txPin = UARTCC32XX_PIN_07_UART1_TX,
        .ctsPin = UARTCC32XX_PIN_UNASSIGNED,
        .rtsPin = UARTCC32XX_PIN_UNASSIGNED,
        .errorFxn = NULL
    }
};

const UART_Config UART_config[BOARD_UART_COUNT] = {
    {
        .fxnTablePtr = &UARTCC32XX_fxnTable,
        .object = &uartCC3220SObjects[BOARD_UART0],
        .hwAttrs = &uartCC3220SHWAttrs[BOARD_UART0]
    },
    {
        .fxnTablePtr = &UARTCC32XX_fxnTable,
        .object = &uartCC3220SObjects[BOARD_UART1],
        .hwAttrs = &uartCC3220SHWAttrs[BOARD_UART1]
    }
};

const uint_least8_t UART_count = BOARD_UART_COUNT;

/*
 *  ============================ Init Functions ============================
 */

void hardware_init()
{
    PRCMCC3200MCUInit();
    Power_init();

    GPIO_init();
    SPI_init();

    shutter_init();
    drum_init();

    buzzer_init();
    leds_init();

    InitTerm();
}

#define TASK_STACK_SIZE (2048)

void dummy_function(_u32 Status, SlDeviceInitInfo_t *DeviceInitInfo) { }

void software_init()
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int ret;
    int detachState;

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    ret = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(ret != 0) while(1);

    pthread_attr_setschedparam(&pAttrs, &priParam);

    ret |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE);
    if(ret != 0) while(1);

//    ret = pthread_create(NULL, &pAttrs, sl_Task, NULL);
//    if(ret != 0) while(1);

//    ret = sl_Start(NULL, NULL, NULL); //dummy_function);
//    if(ret < 0) while(1);

//    configuration_init();
}
