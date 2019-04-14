
#include "hardware.h"
#include "log.h"
#include "schedule.h"

#include <ti/sysbios/knl/Clock.h>
#include <unistd.h>
#include <xdc/runtime/Error.h>

void init_database_polling();
void poll_database();

void * prod_thread(void * arg0)
{
    init_button_interrupts();
    init_database_polling();

    while (1) sleep(1);

    return 0;
}

#define POLL_DATABASE_DELAY_MS (5000)

void init_database_polling()
{
    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = POLL_DATABASE_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(poll_database, POLL_DATABASE_DELAY_MS, &clock_params, Error_IGNORE);
}

void poll_database()
{
    schedule_update();
    log_send_new();
}
