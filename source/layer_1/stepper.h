
#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <stdint.h>

typedef struct
{
    int32_t position;
} stepper_t;

void stepper_init();

void stepper_increment_position(stepper_t * stepper);
void stepper_decrement_position(stepper_t * stepper);

#endif
