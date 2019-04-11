
#include "board.h"

#include "flash.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <unistd.h>

void * at_02_thread(void * arg0)
{
    configuration_t temp;

    while (true)
    {
        temp = configuration_read();
        temp.medication[0].count++;
        UART_PRINT("%d\r\n", temp.medication[0].count);
        configuration_write(temp);
        sleep(2);
        led_status_on();
        sleep(3);
        led_status_off();
    }

    return 0;
}

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

    ret = pthread_create(NULL, &pAttrs, at_02_thread, NULL);
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
