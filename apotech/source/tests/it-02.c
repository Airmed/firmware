
#include "board.h"
#include "buttons.h"
#include "shutter.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>

void * it_02_thread(void * arg0)
{
    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

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

    ret = pthread_create(NULL, &pAttrs, it_02_thread, NULL);
    if(ret != 0) while(1);

    BIOS_start();

    return (0);
}
