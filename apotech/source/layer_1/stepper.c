
#include "stepper.h"

#include "board.h"
#include "pin_map.h"
#include "pwm.h"

#include <stdint.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

xdc_Void stepper_handler(xdc_UArg arg1);

void stepper_init(stepper_t * stepper)
{
    Clock_Params clock_params;

    Clock_Params_init(&clock_params);
    clock_params.period = 100;
    clock_params.startFlag = FALSE;
    clock_params.arg = (xdc_UArg)stepper;
    stepper->timer = Clock_create(stepper_handler, 100, &clock_params, Error_IGNORE);

    stepper_disable(stepper);
}

void stepper_enable(stepper_t * stepper)
{
    GPIO_write(stepper->enable, BOARD_PIN_ON);
}

void stepper_disable(stepper_t * stepper)
{
    GPIO_write(stepper->enable, BOARD_PIN_OFF);
}

void stepper_increment_position(stepper_t * stepper)
{
    switch (stepper->state)
    {
        case STEPPER_STATE_A:
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            stepper->state = STEPPER_STATE_B;
            break;
        case STEPPER_STATE_B:
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            stepper->state = STEPPER_STATE_C;
            break;
        case STEPPER_STATE_C:
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            stepper->state = STEPPER_STATE_D;
            break;
        case STEPPER_STATE_D:
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            stepper->state = STEPPER_STATE_A;
            break;
    }

    stepper->position++;
}

void stepper_decrement_position(stepper_t * stepper)
{
    switch (stepper->state)
    {
        case STEPPER_STATE_A:
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            stepper->state = STEPPER_STATE_D;
            break;
        case STEPPER_STATE_B:
            GPIO_write(stepper->coil0_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil0_n, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            stepper->state = STEPPER_STATE_A;
            break;
        case STEPPER_STATE_C:
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_n, BOARD_PIN_OFF);
            stepper->state = STEPPER_STATE_B;
            break;
        case STEPPER_STATE_D:
            GPIO_write(stepper->coil0_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil0_n, BOARD_PIN_ON);
            GPIO_write(stepper->coil1_p, BOARD_PIN_OFF);
            GPIO_write(stepper->coil1_n, BOARD_PIN_ON);
            stepper->state = STEPPER_STATE_C;
            break;
    }

    stepper->position--;
}

void stepper_start(stepper_t * stepper, int32_t position_target, uint32_t step_delay)
{
    stepper->position_target = position_target;

    stepper_enable(stepper);

    Clock_setPeriod(stepper->timer, step_delay);
    Clock_setTimeout(stepper->timer, step_delay);
    Clock_start(stepper->timer);
}

void stepper_stop(stepper_t * stepper)
{
    Clock_stop(stepper->timer);
    stepper_disable(stepper);
}

void stepper_wait(stepper_t * stepper)
{
    while (Clock_isActive(stepper->timer));
}

xdc_Void stepper_handler(xdc_UArg arg1)
{
    stepper_t * stepper = (stepper_t *)arg1;
    if (stepper->position > stepper->position_target)
    {
        stepper_decrement_position(stepper);
    }
    else if (stepper->position < stepper->position_target)
    {
        stepper_increment_position(stepper);
    }
    else
    {
        stepper_stop(stepper);
    }
}
