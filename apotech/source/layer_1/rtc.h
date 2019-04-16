
#ifndef __RTC_H__
#define __RTC_H__

#include "data_types.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    bool valid;
    uint32_t time_registered;
    uint32_t time;
    void (* callback)(dispense_counts_t);
    dispense_counts_t dispense_counts;
} rtc_dispense_info_t;

void rtc_init();

void rtc_update_time();
void rtc_increment_time();
uint32_t rtc_get_time();
void rtc_register_dispense(uint32_t time, void (* callback)(dispense_counts_t), dispense_counts_t dispense_counts);

uint32_t rtc_utc_to_mdt(uint32_t time);
uint32_t rtc_time_of_day(uint32_t time);
uint32_t rtc_day(uint32_t time);

#endif
