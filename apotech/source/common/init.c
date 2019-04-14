
#include "init.h"

#include "board.h"
#include "buzzer.h"
#include "drum.h"
#include "flash.h"
#include "leds.h"
#include "pin_map.h"
#include "sensor.h"
#include "shutter.h"
#include "uart_term.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/SPI.h>

void hardware_init()
{
    board_init();

    GPIO_init();
    SPI_init();

    shutter_init();
    drum_init();

    buzzer_init();
    leds_init();

    InitTerm();
}

#define TASK_STACK_SIZE (2048)

void software_init()
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int32_t ret;

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 9;
    ret = pthread_attr_setschedparam(&pAttrs, &priParam);
    ret |= pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    ret |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE);
    if(ret != 0) while(1);

    ret = pthread_create(NULL, &pAttrs, sl_Task, NULL);
    if(ret != 0) while(1);

    configuration_init();
}
