
#include "database.h"
#include "file.h"
#include "rtc.h"
#include "uart_term.h"

#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <unistd.h>
#include <xdc/runtime/Error.h>

#define POLL_DATABASE_DELAY_MS (15000)

static bool poll_print_flag = true;

extern void poll_database();
void poll_print_callback();

void * at_02_thread(void * arg0)
{
    Clock_Params clock_params;
    Clock_Params_init(&clock_params);
    clock_params.period = POLL_DATABASE_DELAY_MS;
    clock_params.startFlag = TRUE;
    clock_params.arg = (xdc_UArg)NULL;
    Clock_create(poll_print_callback, POLL_DATABASE_DELAY_MS, &clock_params, Error_IGNORE);

    while (1)
    {
        if (poll_print_flag == true)
        {
            poll_print_flag = false;

            poll_database();

            file_configuration_t configuration = file_configuration_read();
            file_configuration_print(configuration);
        }

        sleep(1);
    }

    return 0;
}

void poll_print_callback()
{
    poll_print_flag = true;
}
