
#include "schedule.h"

#include "database.h"
#include "data_types.h"
#include "file.h"
#include "rtc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void handle_dispenses(dispense_counts_t next_dispense_counts);

void schedule_update()
{
    rtc_update_time();

    configuration_t configuration = database_read_configuration();
    file_configuration_write(configuration);

    schedule_next_dispense();
}

void schedule_next_dispense()
{
//    configuration_t configuration = file_configuration_read();

    configuration_t configuration;
    configuration.medication[0].count = 20;
    configuration.medication[0].dispense_slot[0].valid = true;
    configuration.medication[0].dispense_slot[0].time_of_day = 20700;
    configuration.medication[0].dispense_slot[0].count = 1;
    configuration.medication[0].dispense_slot[1].valid = true;
    configuration.medication[0].dispense_slot[1].time_of_day = 84600;
    configuration.medication[0].dispense_slot[1].count = 2;
    configuration.medication[0].dispense_slot[2].valid = false;
    configuration.medication[0].dispense_slot[2].time_of_day = 84600;
    configuration.medication[0].dispense_slot[2].count = 1;
    configuration.medication[1].count = 15;
    configuration.medication[1].dispense_slot[0].valid = true;
    configuration.medication[1].dispense_slot[0].time_of_day = 100000;
    configuration.medication[1].dispense_slot[0].count = 2;
    configuration.medication[1].dispense_slot[1].valid = true;
    configuration.medication[1].dispense_slot[1].time_of_day = 50000;
    configuration.medication[1].dispense_slot[1].count = 1;
    configuration.medication[1].dispense_slot[2].valid = true;
    configuration.medication[1].dispense_slot[2].time_of_day = 84600;
    configuration.medication[1].dispense_slot[2].count = 1;
    configuration.medication[2].count = 20;
    configuration.medication[2].dispense_slot[0].valid = true;
    configuration.medication[2].dispense_slot[0].time_of_day = 25000;
    configuration.medication[2].dispense_slot[0].count = 1;
    configuration.medication[2].dispense_slot[1].valid = false;
    configuration.medication[2].dispense_slot[1].time_of_day = 84600;
    configuration.medication[2].dispense_slot[1].count = 2;
    configuration.medication[2].dispense_slot[2].valid = false;
    configuration.medication[2].dispense_slot[2].time_of_day = 84600;
    configuration.medication[2].dispense_slot[2].count = 1;
    configuration.medication[3].count = 20;
    configuration.medication[3].dispense_slot[0].valid = false;
    configuration.medication[3].dispense_slot[0].time_of_day = 21000;
    configuration.medication[3].dispense_slot[0].count = 2;
    configuration.medication[3].dispense_slot[1].valid = false;
    configuration.medication[3].dispense_slot[1].time_of_day = 9120;
    configuration.medication[3].dispense_slot[1].count = 2;
    configuration.medication[3].dispense_slot[2].valid = false;
    configuration.medication[3].dispense_slot[2].time_of_day = 84600;
    configuration.medication[3].dispense_slot[2].count = 1;

    uint32_t soonest_dispense_time = -1;
    uint32_t current_time = rtc_time_of_day(rtc_get_time());
    uint32_t current_day = rtc_day(rtc_get_time());
    bool today = true;

    for (uint8_t medication_idx = 0; medication_idx < NUM_MEDICATIONS; medication_idx++)
    {
        for(uint8_t dispense_idx = 0; dispense_idx < NUM_DISPENSE_SLOTS; dispense_idx++)
        {
            bool valid = configuration.medication[medication_idx].dispense_slot[dispense_idx].valid;
            uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;

            if (valid == true && dispense_time > current_time && dispense_time < soonest_dispense_time) soonest_dispense_time = dispense_time;
        }
    }

    if (soonest_dispense_time == -1)
    {
        today = false;
        current_time = 0;

        for (uint8_t medication_idx = 0; medication_idx < NUM_MEDICATIONS; medication_idx++)
        {
            for(uint8_t dispense_idx = 0; dispense_idx < NUM_DISPENSE_SLOTS; dispense_idx++)
            {
                bool valid = configuration.medication[medication_idx].dispense_slot[dispense_idx].valid;
                uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;

                if (valid == true && dispense_time > current_time && dispense_time < soonest_dispense_time) soonest_dispense_time = dispense_time;
            }
        }
    }

    dispense_counts_t dispense_counts;
    dispense_counts.data = 0;

    for (uint8_t medication_idx = 0; medication_idx < NUM_MEDICATIONS; medication_idx++)
    {
        for(uint8_t dispense_idx = 0; dispense_idx < NUM_DISPENSE_SLOTS; dispense_idx++)
        {
            bool valid = configuration.medication[medication_idx].dispense_slot[dispense_idx].valid;
            uint32_t dispense_time = configuration.medication[medication_idx].dispense_slot[dispense_idx].time_of_day;
            uint8_t count = configuration.medication[medication_idx].dispense_slot[dispense_idx].count;

            // TODO handle not enough pills

            if (valid == true && dispense_time == soonest_dispense_time) dispense_counts.medication[medication_idx] += count;
        }
    }

    file_configuration_write(configuration);

    uint32_t next_time;
    if (today == true) next_time = current_day + soonest_dispense_time;
    else next_time = current_day + soonest_dispense_time + 24*60*60;

    rtc_register_dispense(next_time, handle_dispenses, dispense_counts);
}
