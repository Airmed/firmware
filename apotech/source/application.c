
#include "data_types.h"
#include "events.h"
#include "hardware.h"
#include "log.h"
#include "schedule.h"

#include <ti/sysbios/knl/Clock.h>
#include <unistd.h>
#include <xdc/runtime/Error.h>

uint32_t unhandled_events = 0;
dispense_counts_t dispense_counts;

void init_updates();
void handle_updates();
void handle_dispenses(dispense_counts_t next_dispense_counts);

void * prod_thread(void * arg0)
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
            log_send_new();
        }

        if (prev_events & EVENT_DISPENSE)
        {
            dispense();
        }

        usleep(1000);
    }

    return 0;
}

#define POLL_DATABASE_DELAY_MS (15000)

void init_updates()
{
    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = POLL_DATABASE_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(handle_updates, POLL_DATABASE_DELAY_MS, &clock_params, Error_IGNORE);
}

void handle_updates() { unhandled_events |= EVENT_UPDATE; }

void handle_dispenses(dispense_counts_t next_dispense_counts)
{
    unhandled_events |= EVENT_DISPENSE;
    dispense_counts = next_dispense_counts;
}
