
#include "buttons.h"
#include "file.h"
#include "rtc.h"

#include <stdbool.h>
#include <unistd.h>

extern void init_database_polling();

void * at_04_thread(void * arg0)
{
    init_database_polling();

    while (1)
    {
        while (button_patient_get_status() == false) usleep(1000);

        file_log_t log;
        log.time = rtc_get_time();
        log.type = FILE_LOG_TYPE_DISPENSE_ERROR;
        log.notify = true;

        file_log_write(log);

        while (button_patient_get_status() == true) sleep(1);
    }

    return 0;
}
