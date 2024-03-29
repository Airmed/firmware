
#include "hardware.h"

#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "data_types.h"
#include "database.h"
#include "drum.h"
#include "events.h"
#include "file.h"
#include "leds.h"
#include "log.h"
#include "sensor.h"
#include "shutter.h"

#include <ti/drivers/GPIO.h>

extern dispense_counts_t dispense_counts;
extern uint32_t unhandled_events;
extern button_interrupt_e button_interrupt;

void init_button_interrupts()
{
    button_interrupt = BUTTON_INTERRUPT_DISPENSE;
    GPIO_setCallback(BOARD_GPIO_BUTTON_PATIENT, patient_button_callback);
    GPIO_enableInt(BOARD_GPIO_BUTTON_PATIENT);
}

#define BUZZER_FREQ (1000)
#define DISPENSE_RETRIES (2)

void dispense()
{
    uint8_t pills_dispensed;
    bool dispense_failed = false;

    configuration_t configuration = file_configuration_read();

    for (uint8_t medication_idx = 0; medication_idx < NUM_MEDICATIONS; medication_idx++)
    {
        if (dispense_counts.medication[medication_idx] > 0)
        {
            drum_set_hopper(medication_idx);
            pills_dispensed = shutter_dispense();

            for (uint8_t i = 0; i < DISPENSE_RETRIES && pills_dispensed < dispense_counts.medication[medication_idx]; i++)
            {
                pills_dispensed += shutter_dispense();
            }

            if (pills_dispensed != dispense_counts.medication[medication_idx])
            {
                dispense_failed = true;
                configuration.medication[medication_idx].count -= pills_dispensed;
                break;
            }

            configuration.medication[medication_idx].count -= pills_dispensed;
        }
    }

    if (dispense_failed == true)
    {
        led_error_on();
        log_new(LOG_TYPE_DISPENSE_FAILED);
    }
    else
    {
        led_status_on();
        buzzer_set_freq(BUZZER_FREQ);
        button_interrupt = BUTTON_INTERRUPT_SILENCE;
        log_new(LOG_TYPE_DISPENSE_SUCCESS);
    }

    for (uint8_t medication_idx = 0; medication_idx < NUM_MEDICATIONS; medication_idx++)
    {
        database_write_medication_qty(medication_idx + 1, configuration.medication[medication_idx].count);
    }

    dispense_counts.data = 0;
}

#define BUTTON_DEBOUNCE_S (1)

void patient_button_callback(uint_least8_t index)
{
    static uint32_t last_int = 0;

    if (rtc_get_time() < last_int + BUTTON_DEBOUNCE_S) return;

    last_int = rtc_get_time();

    if (button_interrupt == BUTTON_INTERRUPT_DISPENSE)
    {
        unhandled_events |= EVENT_SINGLE_DISPENSE;
    }
    else
    {
        led_status_off();
        buzzer_off();
        button_interrupt = BUTTON_INTERRUPT_DISPENSE;

        unhandled_events |= EVENT_PILLS_TAKEN;
    }
}
