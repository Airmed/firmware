
#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "board.h"

#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>

typedef enum
{
    STEPPER_STATE_A, // [1,0]
    STEPPER_STATE_B, // [0,1]
    STEPPER_STATE_C, // [-1,0]
    STEPPER_STATE_D // [0,-1]
} stepper_state_t;

typedef struct
{
    int32_t position;
    int32_t position_target;
    stepper_state_t state;
    Clock_Handle timer;
    board_gpio_name enable;
    board_gpio_name coil0_p;
    board_gpio_name coil0_n;
    board_gpio_name coil1_p;
    board_gpio_name coil1_n;
} stepper_t;

void stepper_init();

void stepper_enable(stepper_t * stepper);
void stepper_disable(stepper_t * stepper);
void stepper_increment_position(stepper_t * stepper);
void stepper_decrement_position(stepper_t * stepper);

void stepper_start(stepper_t * stepper, int32_t position_target, uint32_t step_delay);
void stepper_stop(stepper_t * stepper);
void stepper_wait(stepper_t * stepper);

#endif
