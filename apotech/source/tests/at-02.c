
#include "board.h"

#include "database.h"
#include "flash.h"
#include "rtc.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <unistd.h>

void * at_02_thread(void * arg0)
{
    while (true);

    return 0;
}
