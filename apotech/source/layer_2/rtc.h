
#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

void rtc_init();

void rtc_update_time();
void rtc_increment_time();
uint32_t rtc_get_time();

#endif
