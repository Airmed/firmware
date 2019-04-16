
#include "board.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdint.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/BIOS.h>

#define __APP_STRING(x) #x
#define _APP_STRING(x) __APP_STRING(x)
#define APP_STRING _APP_STRING(APP_NAME)

#define __APP_THREAD(x) x##_thread
#define _APP_THREAD(x) __APP_THREAD(x)
#define APP_THREAD _APP_THREAD(APP_NAME)

#define THREADSTACKSIZE (8192)

extern void * at_01_thread(void *arg0);
extern void * at_02_thread(void *arg0);
extern void * at_03_thread(void *arg0);
extern void * at_04_thread(void *arg0);
extern void * it_02_thread(void *arg0);
extern void * it_03_thread(void *arg0);
extern void * prod_thread(void *arg0);

void * main_thread(void * arg0)
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int ret;

    hardware_init();

    print_banner(APP_STRING);

    software_init();

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;
    ret = pthread_attr_setschedparam(&pAttrs, &priParam);
    ret |= pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    ret |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    ret |= pthread_create(NULL, &pAttrs, APP_THREAD, NULL);
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
    ret |= pthread_create(NULL, &pAttrs, main_thread, NULL);
    if(ret != 0) while(1);

    BIOS_start();

    return (0);
}
