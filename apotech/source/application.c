
#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "database.h"
#include "drum.h"
#include "file.h"
#include "leds.h"
#include "rtc.h"
#include "sensor.h"
#include "shutter.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <unistd.h>
#include <xdc/runtime/Error.h>

void init_button_interrupts();
void init_database_polling();
void poll_database();
void get_configuration();
void send_logs();
void schedule_next_dispense();
void dispense();
void patient_button_callback(uint_least8_t index);

void * prod_thread(void * arg0)
{
    init_button_interrupts();
    init_database_polling();

    while (1) sleep(1);

    return 0;
}

void init_button_interrupts()
{
    GPIO_setCallback(BOARD_GPIO_BUTTON_PATIENT, patient_button_callback);
}

#define POLL_DATABASE_DELAY_MS (5000)

void init_database_polling()
{
    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = POLL_DATABASE_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(poll_database, POLL_DATABASE_DELAY_MS, &clock_params, Error_IGNORE);
}

void poll_database()
{
    get_configuration();
    send_logs();
}

void get_configuration()
{
    database_medication_t * medication_arr = NULL;
    database_schedule_t * schedule_arr = NULL;

    uint8_t medication_count = database_read_medications(&medication_arr);
    uint8_t schedule_count = database_read_schedule(&schedule_arr);

    file_configuration_t configuration;
    memset(&configuration, 0, sizeof(file_configuration_t));

    // TODO translate

    file_configuration_write(configuration);

    free((void *)medication_arr);
    free((void *)schedule_arr);

    schedule_next_dispense();
}

void send_logs()
{
    file_log_t * log_arr;
    uint8_t log_count = file_log_read(&log_arr);

    for(uint8_t i = 0; i < log_count; i++)
    {
        database_log_t log;

        // TODO translate

        database_write_log(log);
        if (log_arr->notify == true)
        {
            // TODO
        }
    }

    free(log_arr);
}

void schedule_next_dispense()
{
    file_configuration_t configuration = file_configuration_read();

    uint32_t soonest_dispense_time = -1;
    uint32_t current_time = rtc_time_of_day(rtc_get_time());
    uint32_t current_day = rtc_day(rtc_get_time());
    bool today = true;

    bool dispense_arr[FILE_MAX_MEDICATIONS];
    memset(dispense_arr, false, FILE_MAX_MEDICATIONS);

    while (soonest_dispense_time == -1)
    {
        for (uint8_t medication_idx = 0; medication_idx < FILE_MAX_MEDICATIONS; medication_idx++)
        {
            for(uint8_t dispense_idx = 0; dispense_idx < FILE_MAX_DISPENSE_SLOTS; dispense_idx++)
            {
                uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;

                if (dispense_time > current_time && dispense_time < soonest_dispense_time) soonest_dispense_time = dispense_time;
            }
        }

        if (soonest_dispense_time == -1)
        {
            today = false;
            current_time = 0;
        }
        else
        {
            break;
        }
    }

    for (uint8_t medication_idx = 0; medication_idx < FILE_MAX_MEDICATIONS; medication_idx++)
    {
        for(uint8_t dispense_idx = 0; dispense_idx < FILE_MAX_DISPENSE_SLOTS; dispense_idx++)
        {
            uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;

            if (dispense_time == current_time) configuration.medication[medication_idx].next = true;
            else configuration.medication[medication_idx].next = false;
        }
    }

    uint32_t next_time;
    if (today == true) next_time = current_day + soonest_dispense_time;
    else next_time = current_day + soonest_dispense_time + 24*60*60;

    rtc_register_callback(next_time, dispense);
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
