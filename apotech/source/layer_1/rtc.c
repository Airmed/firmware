
#include "rtc.h"

#include "data_types.h"
#include "network.h"
#include "uart_term.h"

#include <stdint.h>
#include <ti/net/sntp/sntp.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

#define RTC_DELAY_MS (1000)

uint32_t rtc_val_s = 0;
uint32_t rtc_val_interrupt = -1;
void (* rtc_callback)() = NULL;

static rtc_dispense_info_t next_dispense;

void rtc_init()
{
    rtc_update_time();

    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = RTC_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(rtc_increment_time, RTC_DELAY_MS, &clock_params, Error_IGNORE);

    next_dispense.valid = false;
}

void rtc_update_time()
{
    int32_t ret;
    uint64_t curr_time;
    SlNetSock_Timeval_t timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    ret = SNTP_getTime(NULL, 0, &timeout, &curr_time);
    if (ret == 0)
    {
        rtc_val_s = rtc_utc_to_mdt((curr_time >> 32) & 0xFFFFFFFF);
        UART_PRINT("time updated: %u\n\r", rtc_val_s);
    }
}

void rtc_increment_time()
{
    rtc_val_s++;

    if (next_dispense.valid == true && rtc_val_s >= next_dispense.time)
    {
        next_dispense.valid = false;
        next_dispense.callback(next_dispense.dispense_counts);
    }

}

uint32_t rtc_get_time()
{
    return rtc_val_s;
}

void rtc_register_dispense(uint32_t time, void (* callback)(dispense_counts_t), dispense_counts_t dispense_counts)
{
    if (next_dispense.valid != true || next_dispense.time > time)
    {
        next_dispense.valid = true;
        next_dispense.time_registered = rtc_val_s;
        next_dispense.time = time;
        next_dispense.callback = callback;
        next_dispense.dispense_counts = dispense_counts;
    }
}

uint32_t rtc_utc_to_mdt(uint32_t time)
{
    return time - 6*60*60;
}

uint32_t rtc_time_of_day(uint32_t time)
{
    return time % (24*60*60);
}

uint32_t rtc_day(uint32_t time)
{
    return time - rtc_time_of_day(time);
}
