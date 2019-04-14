
#include "buttons.h"
#include "log.h"

#include <stdbool.h>
#include <unistd.h>

extern void init_database_polling();

void * at_04_thread(void * arg0)
{
    init_database_polling();

    while (1)
    {
        while (button_patient_get_status() == false) usleep(1000);

        log_new(LOG_TYPE_DISPENSE_ERROR);

        while (button_patient_get_status() == true) sleep(1);
    }

    return 0;
}
