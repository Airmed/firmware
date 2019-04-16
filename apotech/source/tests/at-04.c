
#include "buttons.h"
#include "events.h"
#include "log.h"

#include <stdbool.h>
#include <unistd.h>

extern uint32_t unhandled_events;

extern void init_updates();

void * at_04_thread(void * arg0)
{
    init_updates();

    while (1)
    {
        uint32_t prev_events = unhandled_events;
        unhandled_events &= ~prev_events;

        if (prev_events & EVENT_UPDATE)
        {
            log_send_new();
        }

        if (button_patient_get_status() == true)
        {
            while (button_patient_get_status() == true) usleep(10000);
        }

        usleep(1000);
    }

    return 0;
}
