
#include "schedule.h"

#include "database.h"
#include "file.h"
#include "rtc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void dispense();

void schedule_update()
{
    rtc_update_time();

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

void schedule_next_dispense()
{
//    file_configuration_t configuration = file_configuration_read();
//
//    uint32_t soonest_dispense_time = -1;
//    uint32_t current_time = rtc_time_of_day(rtc_get_time());
//    uint32_t current_day = rtc_day(rtc_get_time());
//    bool today = true;
//
//    bool dispense_arr[FILE_MAX_MEDICATIONS];
//    memset(dispense_arr, false, FILE_MAX_MEDICATIONS);
//
//    while (soonest_dispense_time == -1)
//    {
//        for (uint8_t medication_idx = 0; medication_idx < FILE_MAX_MEDICATIONS; medication_idx++)
//        {
//            for(uint8_t dispense_idx = 0; dispense_idx < FILE_MAX_DISPENSE_SLOTS; dispense_idx++)
//            {
//                uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;
//
//                if (dispense_time > current_time && dispense_time < soonest_dispense_time) soonest_dispense_time = dispense_time;
//            }
//        }
//
//        if (soonest_dispense_time == -1)
//        {
//            today = false;
//            current_time = 0;
//        }
//        else
//        {
//            break;
//        }
//    }
//
//    for (uint8_t medication_idx = 0; medication_idx < FILE_MAX_MEDICATIONS; medication_idx++)
//    {
//        for(uint8_t dispense_idx = 0; dispense_idx < FILE_MAX_DISPENSE_SLOTS; dispense_idx++)
//        {
//            uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;
//
//            if (dispense_time == current_time) configuration.medication[medication_idx].next = true;
//            else configuration.medication[medication_idx].next = false;
//        }
//    }
//
//    uint32_t next_time;
//    if (today == true) next_time = current_day + soonest_dispense_time;
//    else next_time = current_day + soonest_dispense_time + 24*60*60;
//
//    rtc_register_callback(next_time, dispense);
}
