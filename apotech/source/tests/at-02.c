
#include "board.h"

#include "flash.h"
#include "init.h"
#include "network.h"
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

void print_banner(char * application)
{
    UART_PRINT("\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("\t=============================================\n\r");
    UART_PRINT("\t\t%s\n\r", application);
    UART_PRINT("\t=============================================\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("\n\r");
}

#define THREADSTACKSIZE (4096)

void * main_thread(void * arg0)
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int ret;

    hardware_init();

    print_banner("AT-02");

    software_init();
    network_connect();

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    ret = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    ret |= pthread_attr_setschedparam(&pAttrs, &priParam);
    ret |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if(ret != 0) while(1);

    ret = pthread_create(NULL, &pAttrs, at_02_thread, NULL);
    if(ret != 0) while(1);

    return 0;
}

int main()
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int ret;

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    ret = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    ret |= pthread_attr_setschedparam(&pAttrs, &priParam);
    ret |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if(ret != 0) while(1);

    ret = pthread_create(NULL, &pAttrs, main_thread, NULL);
    if(ret != 0) while(1);

    BIOS_start();

    return (0);
}
