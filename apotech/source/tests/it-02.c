
#include "gpio.h"
#include "shutter.h"

extern void hardware_init();

void * it_02_thread(void * arg0)
{
    hardware_init();

    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}
