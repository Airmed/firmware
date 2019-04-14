
#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

void rtc_init();

void rtc_update_time();
void rtc_increment_time();
uint32_t rtc_get_time();
void rtc_register_callback(uint32_t time, void (* callback)(void));

uint32_t rtc_utc_to_mdt(uint32_t time);
uint32_t rtc_time_of_day(uint32_t time);
uint32_t rtc_day(uint32_t time);

#endif
