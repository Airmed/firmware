
#include "board.h"
#include "drum.h"
#include "gpio.h"
#include "shutter.h"

#include <pthread.h>
#include <ti/sysbios/BIOS.h>

extern void hardware_init();

#define PILLS_PER_HOPPER (10)

void * at_01_thread(void * arg0)
{
    shutter_dispense_t ret;

    hardware_init();

    while (true)
    {
        for (uint8_t pills_dispensed = 0; pills_dispensed < PILLS_PER_HOPPER; pills_dispensed++)
        {
            while (button_patient_get_status() == false);

            ret = shutter_dispense();
            while (ret == SHUTTER_DISPENSE_NONE)
            {
                ret = shutter_dispense();
            }

            if (ret == SHUTTER_DISPENSE_SUCCESS)
            {
                led_status_on();
                led_error_off();
            }
            else if (ret == SHUTTER_DISPENSE_MULTIPLE)
            {
                led_status_off();
                led_error_on();
            }
        }

        drum_next_hopper();
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

    retc = pthread_create(&application, &pAttrs, at_01_thread, NULL);
    if(retc != 0) while(1);

    BIOS_start();

    return (0);
}
