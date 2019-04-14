
#include "hardware.h"

#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "drum.h"
#include "file.h"
#include "leds.h"
#include "sensor.h"
#include "shutter.h"

#include <ti/drivers/GPIO.h>

void init_button_interrupts()
{
    GPIO_setCallback(BOARD_GPIO_BUTTON_PATIENT, patient_button_callback);
}

#define BUZZER_FREQ (1000)

void dispense()
{
    file_configuration_t configuration = file_configuration_read();
    bool dispense_failed = false;

    for (uint8_t medication_idx = 0; medication_idx < FILE_MAX_MEDICATIONS; medication_idx++)
    {
        if (configuration.medication[medication_idx].next == true)
        {
            drum_set_hopper(medication_idx);
            shutter_dispense_t ret = shutter_dispense();

            while (ret == SHUTTER_DISPENSE_NONE) ret = shutter_dispense();

            if (ret == SHUTTER_DISPENSE_MULTIPLE)
            {
                dispense_failed = true;
                break;
            }
        }
    }

    if (dispense_failed == true)
    {
        led_error_on();
    }
    else
    {
        led_status_on();
        buzzer_set_freq(BUZZER_FREQ);
        GPIO_enableInt(BOARD_GPIO_BUTTON_PATIENT);
    }
}

void patient_button_callback(uint_least8_t index)
{
    GPIO_disableInt(index);
    led_status_off();
    buzzer_off();
}
