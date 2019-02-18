
#ifndef __DRUM_H__
#define __DRUM_H__

typedef enum
{
    DRUM_HOPPER_0,
    DRUM_HOPPER_1,
    DRUM_HOPPER_2,
    DRUM_HOPPER_3
} drum_hoppers_e;

void drum_init();

void drum_set_hopper(drum_hoppers_e hopper);

#endif
