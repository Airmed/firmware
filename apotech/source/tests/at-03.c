
#include "board.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>

void * at_03_thread(void * arg0)
{
    while (true);

    return 0;
}

#define THREADSTACKSIZE (4096)

int main()
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int ret;

    hardware_init();
    software_init();

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    ret = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    ret |= pthread_attr_setschedparam(&pAttrs, &priParam);
    ret |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if(ret != 0) while(1);

    ret = pthread_create(NULL, &pAttrs, at_03_thread, NULL);
    if(ret != 0) while(1);

    BIOS_start();

    return (0);
}
