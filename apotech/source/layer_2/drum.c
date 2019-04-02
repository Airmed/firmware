
#include "drum.h"

#include "pin_map.h"
#include "stepper.h"

stepper_t drum_motor;

void drum_init()
{
    drum_motor.position = 0;
    drum_motor.position_target = 0;
    drum_motor.enable = BOARD_DRUM_EN;
    drum_motor.sleep = BOARD_DRUM_SLEEP;
    drum_motor.reset = BOARD_DRUM_RESET;
    drum_motor.dir = BOARD_DRUM_DIR;
    drum_motor.step = BOARD_DRUM_STEP;

    stepper_init(&drum_motor);
}

#define DRUM_HOPPER_0_POS (0)
#define DRUM_HOPPER_1_POS (178)
#define DRUM_HOPPER_2_POS (356)
#define DRUM_HOPPER_3_POS (533)
#define DRUM_STEP_DELAY_MS (200)

void drum_set_hopper(drum_hoppers_e hopper)
{
    int32_t pos;
    switch (hopper)
    {
        case DRUM_HOPPER_0:
            pos = DRUM_HOPPER_0_POS;
            break;
        case DRUM_HOPPER_1:
            pos = DRUM_HOPPER_1_POS;
            break;
        case DRUM_HOPPER_2:
            pos = DRUM_HOPPER_2_POS;
            break;
        case DRUM_HOPPER_3:
            pos = DRUM_HOPPER_3_POS;
            break;
        default:
            pos = drum_motor.position;
            break;
    }

    stepper_start(&drum_motor, pos, DRUM_STEP_DELAY_MS);
    stepper_wait(&drum_motor);
}

void drum_next_hopper()
{
    static drum_hoppers_e hopper = DRUM_HOPPER_0;

    switch (hopper)
    {
        case DRUM_HOPPER_0:
            hopper = DRUM_HOPPER_1;
            break;
        case DRUM_HOPPER_1:
            hopper = DRUM_HOPPER_2;
            break;
        case DRUM_HOPPER_2:
            hopper = DRUM_HOPPER_3;
            break;
        case DRUM_HOPPER_3:
            hopper = DRUM_HOPPER_0;
            break;
    }

    drum_set_hopper(hopper);
}
