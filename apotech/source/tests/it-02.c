
#include "buttons.h"
#include "shutter.h"

void * it_02_thread(void * arg0)
{
    while (1)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}
