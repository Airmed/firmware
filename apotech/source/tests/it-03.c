
#include "board.h"
#include "drum.h"
#include "gpio.h"

#include <pthread.h>
#include <ti/sysbios/BIOS.h>

extern void hardware_init();

void * it_03_thread(void * arg0)
{
    drum_hoppers_e hopper_next = DRUM_HOPPER_1;

    hardware_init();

    while (true)
    {
        while (button_patient_get_status() == false);

        drum_set_hopper(hopper_next);
        switch (hopper_next)
        {
            case DRUM_HOPPER_0:
                hopper_next = DRUM_HOPPER_1;
                break;
            case DRUM_HOPPER_1:
                hopper_next = DRUM_HOPPER_2;
                break;
            case DRUM_HOPPER_2:
                hopper_next = DRUM_HOPPER_3;
                break;
            case DRUM_HOPPER_3:
                hopper_next = DRUM_HOPPER_0;
                break;
        }
    }

    return 0;
}

/* Stack size in bytes */
#define THREADSTACKSIZE    4096

int main(void)
{
    pthread_t application;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Call board init functions */
    board_init();

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0) while(1);

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if(retc != 0) while(1);

    retc = pthread_create(&application, &pAttrs, it_03_thread, NULL);
    if(retc != 0) while(1);

    BIOS_start();

    return (0);
}
