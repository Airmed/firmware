
#include "buttons.h"
#include "drum.h"

void * it_03_thread(void * arg0)
{
    drum_hoppers_e hopper_next = DRUM_HOPPER_1;

    while (1)
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
