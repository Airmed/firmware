
#include "log.h"

#include "database.h"
#include "file.h"
#include "rtc.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void log_new(log_type_t type)
{
    file_log_t log;
    log.header.type = (uint8_t)type;
    log.header.time = rtc_get_time();

    if (type == LOG_TYPE_DISPENSE_ERROR) log.header.notify = true;
    else log.header.notify = false;

    file_log_write(log);
}

void log_send_new()
{
    file_log_t * log_arr;
    uint8_t log_count = file_log_read(&log_arr);

    for(uint8_t i = 0; i < log_count; i++)
    {
        database_log_t log;

        // TODO translate

        database_write_log(log);
        if (log_arr[i].header.notify == true)
        {
            // TODO
        }
    }

    free(log_arr);
}
