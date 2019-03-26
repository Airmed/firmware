
#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "board.h"

#include <stdbool.h>
#include <stdint.h>
#include "ti/drivers/ADC.h"

typedef struct
{
    uint16_t reference_val;
    board_gpio_name pin;
    board_adc_name adc;
    ADC_Handle adc_handle;
} sensor_t;

void sensor_init(sensor_t * sensor);

bool sensor_get_status(sensor_t * sensor);

#endif
