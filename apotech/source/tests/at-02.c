
#include "board.h"

#include "database.h"
#include "flash.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <unistd.h>

void * at_02_thread(void * arg0)
{
    database_init();
    database_read_medications(NULL);
    database_read_schedule(NULL);
    database_deinit();

    while (true);

    return 0;
}
