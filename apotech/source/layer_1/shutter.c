
#include "shutter.h"

#include "board.h"
#include "pin_map.h"
#include "sensor.h"
#include "stepper.h"

#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <unistd.h>
#include <xdc/runtime/Error.h>

sensor_t sensor_high;
sensor_t sensor_low;
stepper_t shutter_motor;

xdc_Void shutter_count(xdc_UArg arg1);

#define SENSOR_HIGH_THRESHOLD (1.5f)
#define SENSOR_LOW_THRESHOLD (1.3f)

void shutter_init()
{
    sensor_high.pin = BOARD_SENSOR_HIGH;
    sensor_high.adc = BOARD_SENSOR_HIGH_ADC;
    sensor_init(&sensor_high, SENSOR_HIGH_THRESHOLD);

    sensor_low.pin = BOARD_SENSOR_LOW;
    sensor_low.adc = BOARD_SENSOR_LOW_ADC;
    sensor_init(&sensor_low, SENSOR_LOW_THRESHOLD);

    shutter_motor.position = 0;
    shutter_motor.position_target = 0;
    shutter_motor.enable = BOARD_SHUTTER_EN;
    shutter_motor.sleep = BOARD_SHUTTER_SLEEP;
    shutter_motor.reset = BOARD_SHUTTER_RESET;
    shutter_motor.dir = BOARD_SHUTTER_DIR;
    shutter_motor.step = BOARD_SHUTTER_STEP;

    stepper_init(&shutter_motor);
}

#define STEP_COUNT_MAX (90)
#define SLOW_STEP_DELAY_MS (100)
#define FAST_STEP_DELAY_MS (2)
#define SENSED_STEPS_OPEN (2)
#define SHUTTER_OPEN_DELAY_US (10000)
#define VIBRATE_REPS (3)
#define VIBRATE_STEP_COUNT (10)
#define STEP_COUNT_OVERSHOOT (32)
#define PILL_COUNT_DELAY_US (1000000)

uint8_t shutter_dispense()
{
    bool sensor_status = false, sensor_status_last = false;
    volatile uint8_t pills_dispensed = 0;

    Clock_Params clock_params;

    Clock_Params_init(&clock_params);
    clock_params.period = 1;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)&pills_dispensed;
    Clock_Handle pill_count_clock = Clock_create(shutter_count, 1, &clock_params, Error_IGNORE);

    for (uint8_t i = 0; i < VIBRATE_REPS; i++)
    {
        stepper_start(&shutter_motor, VIBRATE_STEP_COUNT, FAST_STEP_DELAY_MS);
        stepper_wait(&shutter_motor);
        stepper_start(&shutter_motor, 0, FAST_STEP_DELAY_MS);
        stepper_wait(&shutter_motor);
    }

    stepper_start(&shutter_motor, STEP_COUNT_MAX, SLOW_STEP_DELAY_MS);

    while (shutter_motor.position < STEP_COUNT_MAX && pills_dispensed == 0)
    {
        sensor_status_last = sensor_status;
        sensor_status = sensor_get_status(&sensor_high);
        if (sensor_status == true && sensor_status_last == true)
        {
            stepper_start(&shutter_motor, shutter_motor.position + SENSED_STEPS_OPEN, FAST_STEP_DELAY_MS);
            stepper_wait(&shutter_motor);
            usleep(SHUTTER_OPEN_DELAY_US);
            break;
        }
    }

    stepper_start(&shutter_motor, -STEP_COUNT_OVERSHOOT, FAST_STEP_DELAY_MS);
    stepper_wait(&shutter_motor);

    shutter_motor.position = 0;

    usleep(PILL_COUNT_DELAY_US);
    Clock_delete(&pill_count_clock);

    return pills_dispensed;
}

xdc_Void shutter_count(xdc_UArg arg1)
{
    volatile uint8_t * ptr_pills_dispensed = (volatile uint8_t *)arg1;
    static bool sensor_status_last;

    bool sensor_status = sensor_get_status(&sensor_low);

    if (sensor_status == true && sensor_status_last == false)
    {
        (*ptr_pills_dispensed)++;
    }

    sensor_status_last = sensor_status;
}
