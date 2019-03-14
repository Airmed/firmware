
#include "shutter.h"

#include "board.h"
#include "pin_map.h"
#include "sensor.h"
#include "stepper.h"

#include <unistd.h>

stepper_t shutter_motor;

void shutter_init()
{
    sensor_init();

    shutter_motor.position = 0;
    shutter_motor.position_target = 0;
    shutter_motor.state = STEPPER_STATE_A;
    shutter_motor.enable = BOARD_SHUTTER_EN;
    shutter_motor.coil0_p = BOARD_SHUTTER_COIL0_P;
    shutter_motor.coil0_n = BOARD_SHUTTER_COIL0_N;
    shutter_motor.coil1_p = BOARD_SHUTTER_COIL1_P;
    shutter_motor.coil1_n = BOARD_SHUTTER_COIL1_N;

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

void shutter_dispense()
{
    bool sensor_status = false, sensor_status_last = false;

    for (uint8_t i = 0; i < VIBRATE_REPS; i++)
    {
        stepper_start(&shutter_motor, VIBRATE_STEP_COUNT, FAST_STEP_DELAY_MS);
        stepper_wait(&shutter_motor);
        stepper_start(&shutter_motor, 0, FAST_STEP_DELAY_MS);
        stepper_wait(&shutter_motor);
    }

    stepper_start(&shutter_motor, STEP_COUNT_MAX, SLOW_STEP_DELAY_MS);

    while (shutter_motor.position < STEP_COUNT_MAX)
    {
        sensor_status_last = sensor_status;
        sensor_status = sensor_get_status();
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
}
