
#include "buttons.h"
#include "drum.h"
#include "leds.h"
#include "shutter.h"

#include <stdint.h>

#define PILLS_PER_HOPPER (10)
#define DISPENSE_RETRIES (2)

void * at_01_thread(void * arg0)
{
    shutter_dispense_t ret;

    while (1)
    {
        for (uint8_t pills_dispensed = 0; pills_dispensed < PILLS_PER_HOPPER; pills_dispensed++)
        {
            while (button_patient_get_status() == false);
            led_status_off();
            led_error_off();

            ret = shutter_dispense();
            for (uint8_t i = 0; i < DISPENSE_RETRIES && ret == SHUTTER_DISPENSE_NONE; i++)
            {
                ret = shutter_dispense();
            }

            if (ret == SHUTTER_DISPENSE_NONE)
            {
                led_status_off();
                led_error_on();
                break; // hopper is empty
            }
            else if (ret == SHUTTER_DISPENSE_SUCCESS)
            {
                led_status_on();
                led_error_off();
            }
            else if (ret == SHUTTER_DISPENSE_MULTIPLE)
            {
                led_status_off();
                led_error_on();
            }
        }

        drum_next_hopper();
    }

    return 0;
}
