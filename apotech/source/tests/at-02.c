
#include "board.h"

#include "database.h"
#include "flash.h"
#include "rtc.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include <unistd.h>

#define POLL_DATABASE_DELAY_MS (5000)

extern void poll_database();
void poll_print_database();

void * at_02_thread(void * arg0)
{
    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = POLL_DATABASE_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(poll_print_database, POLL_DATABASE_DELAY_MS, &clock_params, Error_IGNORE);

    while (true);

    return 0;
}

void poll_print_database()
{
    poll_database();

    configuration_t configuration = configuration_read();
    configuration_print(configuration);
}
