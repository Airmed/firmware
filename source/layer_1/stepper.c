
#include "stepper.h"

#include "pin_map.h"
#include "pwm.h"

#include <ti/drivers/GPIO.h>

#include "board.h"

#include <stdint.h>

void stepper_init()
{

}

void stepper_increment_position(stepper_t * stepper)
{
    switch (stepper->state)
    {
        case STEPPER_STATE_A:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
        case STEPPER_STATE_B:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
        case STEPPER_STATE_C:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            break;
        case STEPPER_STATE_D:
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
    }

    stepper->position++;
}

void stepper_decrement_position(stepper_t * stepper)
{
    switch (stepper->state)
    {
        case STEPPER_STATE_A:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            break;
        case STEPPER_STATE_B:
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
        case STEPPER_STATE_C:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
        case STEPPER_STATE_D:
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            break;
    }

    stepper->position--;
}
