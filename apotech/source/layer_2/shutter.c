
#include "shutter.h"

#include "pin_map.h"
#include "sensor.h"
#include "stepper.h"

#include "board.h"
#include "ti/drivers/GPIO.h"
#include <unistd.h>

stepper_t shutter;

void shutter_init()
{
    sensor_init();

    shutter.position = 0;
    shutter.position_target = 0;
    shutter.state = STEPPER_STATE_A;
    shutter.enable = BOARD_SHUTTER_EN;
    shutter.coil0_p = BOARD_SHUTTER_COIL0_P;
    shutter.coil0_n = BOARD_SHUTTER_COIL0_N;
    shutter.coil1_p = BOARD_SHUTTER_COIL1_P;
    shutter.coil1_n = BOARD_SHUTTER_COIL1_N;

    GPIO_write(shutter.enable, BOARD_PIN_ON);
    GPIO_write(shutter.coil0_p, BOARD_PIN_ON);
    GPIO_write(shutter.coil0_n, BOARD_PIN_OFF);
    GPIO_write(shutter.coil1_p, BOARD_PIN_OFF);
    GPIO_write(shutter.coil1_n, BOARD_PIN_OFF);

    stepper_init(&shutter);
}

#define STEP_COUNT_MAX (90)
#define SLOW_STEP_DELAY_MS (100)
#define FAST_STEP_DELAY_MS (2)
#define SENSED_STEPS_OPEN (10)
#define SHUTTER_OPEN_DELAY_US (10000)

void shutter_dispense()
{
    bool sensor_status = false, sensor_status_last = false;

    stepper_start(&shutter, STEP_COUNT_MAX, SLOW_STEP_DELAY_MS);

    while (sensor_status == false && sensor_status_last == false)
    {
        sensor_status_last = sensor_status;
        sensor_status = sensor_get_status();
    }

    stepper_start(&shutter, shutter.position + SENSED_STEPS_OPEN, FAST_STEP_DELAY_MS);
    stepper_wait(&shutter);
    usleep(SHUTTER_OPEN_DELAY_US);

    stepper_start(&shutter, 0, FAST_STEP_DELAY_MS);
    stepper_wait(&shutter);
}
