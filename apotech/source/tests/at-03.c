
#include "events.h"
#include "file.h"
#include "hardware.h"
#include "schedule.h"

#include <unistd.h>

extern uint32_t unhandled_events;

extern void init_button_interrupts();
extern void init_updates();

void * at_03_thread(void * arg0)
{
    init_button_interrupts();
    init_updates();

    while (1)
    {
        uint32_t prev_events = unhandled_events;
        unhandled_events &= ~prev_events;

        if (prev_events & EVENT_UPDATE)
        {
            schedule_update();

            configuration_t configuration = file_configuration_read();
            file_configuration_print(configuration);
        }

        if (prev_events & EVENT_DISPENSE)
        {
            dispense();
        }

        usleep(1000);
    }

    return 0;
}
