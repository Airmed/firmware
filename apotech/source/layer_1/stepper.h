
#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "board.h"

#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>

typedef struct
{
    int32_t position;
    int32_t position_target;
    Clock_Handle timer;
    board_gpio_name enable;
    board_gpio_name sleep;
    board_gpio_name reset;
    board_gpio_name dir;
    board_gpio_name step;
} stepper_t;

void stepper_init();

void stepper_enable(stepper_t * stepper);
void stepper_disable(stepper_t * stepper);
void stepper_reset(stepper_t * stepper);
void stepper_increment_position(stepper_t * stepper);
void stepper_decrement_position(stepper_t * stepper);

void stepper_start(stepper_t * stepper, int32_t position_target, uint32_t step_delay);
void stepper_stop(stepper_t * stepper);
void stepper_wait(stepper_t * stepper);

#endif
