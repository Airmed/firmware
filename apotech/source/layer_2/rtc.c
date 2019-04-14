
#include "rtc.h"

#include "network.h"
#include "uart_term.h"

#include <stdint.h>
#include <ti/net/sntp/sntp.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

#define RTC_DELAY_MS (1000)

uint32_t rtc_val_s = 0;

void rtc_init()
{
    network_connect();
    rtc_update_time();

    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = RTC_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(rtc_increment_time, RTC_DELAY_MS, &clock_params, Error_IGNORE);
}

void rtc_update_time()
{
    int32_t ret;
    uint64_t curr_time;
    SlNetSock_Timeval_t timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    static const int num_servers = 1;
    const char * servers[num_servers];
    servers[0] = "129.6.15.28";

    ret = SNTP_getTime(servers, num_servers, &timeout, &curr_time);
    if (ret == 0)
    {
        rtc_val_s = (curr_time >> 32) & 0xFFFFFFFF;
    }
}

void rtc_increment_time()
{
    rtc_val_s++;
}

uint32_t rtc_get_time()
{
    return rtc_val_s;
}
