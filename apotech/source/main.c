
#include "board.h"

#include <pthread.h>
#include <stdint.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/BIOS.h>

extern void * provisioning_task(void *arg0);
extern void * application_thread(void *arg0);

#define THREADSTACKSIZE (4096)

void * main_thread(void * arg0)
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

//    ret = pthread_create(NULL, &pAttrs, provisioning_task, NULL);
//    if(ret != 0) while(1);

    ret = pthread_create(NULL, &pAttrs, application_thread, NULL);
    if(ret != 0) while(1);
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
