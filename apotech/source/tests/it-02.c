
#include "gpio.h"
#include "shutter.h"

extern void hardware_init();

int main(void)
{
    hardware_init();

    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}
