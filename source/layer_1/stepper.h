
#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <stdint.h>

#include "board.h"

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
    stepper_state_t state;
    board_gpio_name enable;
    board_gpio_name coil0_p;
    board_gpio_name coil0_n;
    board_gpio_name coil1_p;
    board_gpio_name coil1_n;
} stepper_t;

void stepper_init();

void stepper_increment_position(stepper_t * stepper);
void stepper_decrement_position(stepper_t * stepper);

#endif
