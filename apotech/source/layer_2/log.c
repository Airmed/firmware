
#include "log.h"

#include "database.h"
#include "file.h"
#include "rtc.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void log_new(log_type_e type)
{
    log_t log;
    log.type = type;
    log.time = rtc_get_time();

    if (type == LOG_TYPE_DISPENSE_FAILED) log.notify = true;
    else log.notify = false;

    file_log_write(log);
}

void log_send_new()
{
    log_t * log_arr;
    uint8_t log_count = file_log_read(&log_arr);

    for(uint8_t i = 0; i < log_count; i++)
    {
        database_write_log(log_arr[i], log_arr[i].notify);
    }

    if (log_count > 0) free(log_arr);
}
