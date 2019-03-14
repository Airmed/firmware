
#include "drum.h"
#include "gpio.h"

extern void hardware_init();

void * it_03_thread(void * arg0)
{
    drum_hoppers_e hopper_next = DRUM_HOPPER_1;

    hardware_init();

    while (true)
    {
        while (button_patient_get_status() == false);

        drum_set_hopper(hopper_next);
        switch (hopper_next)
        {
            case DRUM_HOPPER_0:
                hopper_next = DRUM_HOPPER_1;
                break;
            case DRUM_HOPPER_1:
                hopper_next = DRUM_HOPPER_2;
                break;
            case DRUM_HOPPER_2:
                hopper_next = DRUM_HOPPER_3;
                break;
            case DRUM_HOPPER_3:
                hopper_next = DRUM_HOPPER_0;
                break;
        }
    }

    return 0;
}
