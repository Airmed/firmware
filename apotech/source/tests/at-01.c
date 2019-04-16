
#include "buttons.h"
#include "drum.h"
#include "leds.h"
#include "shutter.h"

#include <stdint.h>

#define PILLS_PER_HOPPER (10)
#define DISPENSE_RETRIES (2)

void * at_01_thread(void * arg0)
{
    uint8_t pills_dispensed;

    while (1)
    {
        for (uint8_t pill = 0; pill < PILLS_PER_HOPPER; pill++)
        {
            while (button_patient_get_status() == false);
            led_status_off();
            led_error_off();

            pills_dispensed = shutter_dispense();
            for (uint8_t i = 0; i < DISPENSE_RETRIES && pills_dispensed == 0; i++)
            {
                pills_dispensed = shutter_dispense();
            }

            if (pills_dispensed == 0)
            {
                led_status_off();
                led_error_on();
                break; // hopper is empty
            }
            else if (pills_dispensed == 1)
            {
                led_status_on();
                led_error_off();
            }
            else if (pills_dispensed > 1)
            {
                led_status_off();
                led_error_on();
            }
        }

        drum_next_hopper();
    }

    return 0;
}
