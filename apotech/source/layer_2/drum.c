
#include "drum.h"

#include "pin_map.h"
#include "stepper.h"

stepper_t drum_motor;

void drum_init()
{
    drum_motor.position = 0;
    drum_motor.position_target = 0;
    drum_motor.state = STEPPER_STATE_A;
    drum_motor.enable = BOARD_DRUM_EN;
    drum_motor.coil0_p = BOARD_DRUM_COIL0_P;
    drum_motor.coil0_n = BOARD_DRUM_COIL0_N;
    drum_motor.coil1_p = BOARD_DRUM_COIL1_P;
    drum_motor.coil1_n = BOARD_DRUM_COIL1_N;

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
