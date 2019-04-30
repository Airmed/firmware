
#include "stepper.h"

#include "board.h"
#include "pin_map.h"

#include <stdint.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

#define STEPPER_DIR_CW (BOARD_PIN_OFF)
#define STEPPER_DIR_CCW (BOARD_PIN_ON)

xdc_Void stepper_handler(xdc_UArg arg1);

void stepper_init(stepper_t * stepper)
{
    Clock_Params clock_params;

    Clock_Params_init(&clock_params);
    clock_params.period = 100;
    clock_params.startFlag = FALSE;
    clock_params.arg = (xdc_UArg)stepper;
    stepper->timer = Clock_create(stepper_handler, 100, &clock_params, Error_IGNORE);

    GPIO_write(stepper->sleep, BOARD_PIN_ON);
    GPIO_write(stepper->dir, BOARD_PIN_OFF);
    GPIO_write(stepper->step, BOARD_PIN_OFF);

    stepper_reset(stepper);
    stepper_disable(stepper);
}

void stepper_enable(stepper_t * stepper)
{
    GPIO_write(stepper->enable, BOARD_PIN_OFF);
}

void stepper_disable(stepper_t * stepper)
{
    GPIO_write(stepper->enable, BOARD_PIN_ON);
}

void stepper_reset(stepper_t * stepper)
{
    GPIO_write(stepper->reset, BOARD_PIN_OFF);
    GPIO_write(stepper->reset, BOARD_PIN_ON);
}

void stepper_increment_position(stepper_t * stepper)
{
    if (GPIO_read(stepper->dir) == STEPPER_DIR_CCW) GPIO_write(stepper->dir, STEPPER_DIR_CW);

    GPIO_write(stepper->step, BOARD_PIN_ON);
    GPIO_write(stepper->step, BOARD_PIN_OFF);

    stepper->position++;
}

void stepper_decrement_position(stepper_t * stepper)
{
    if (GPIO_read(stepper->dir) == STEPPER_DIR_CW) GPIO_write(stepper->dir, STEPPER_DIR_CCW);

    GPIO_write(stepper->step, BOARD_PIN_ON);
    GPIO_write(stepper->step, BOARD_PIN_OFF);

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
