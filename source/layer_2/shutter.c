
#include "shutter.h"

#include "pin_map.h"
#include "sensor.h"
#include "stepper.h"

#include "board.h"
#include "ti/drivers/GPIO.h"

stepper_t shutter;

void shutter_init()
{
    sensor_init();

    shutter.position = 0;
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
}

void shutter_dispense()
{
    stepper_increment_position(&shutter);
}
