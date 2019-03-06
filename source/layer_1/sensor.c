
#include "gpio.h"

#include "pin_map.h"

#include "board.h"
#include <stdbool.h>
#include "ti/drivers/ADC.h"

static ADC_Handle sensor_adc;
static uint16_t sensor_val_ref;

void sensor_init()
{
    ADC_Params params;

    ADC_init();
    ADC_Params_init(&params);
    sensor_adc = ADC_open(0, &params);
    if (sensor_adc == 0) while (1);
    ADC_convert(sensor_adc, &sensor_val_ref);
    sensor_val_ref *= 1.5f;
}

bool sensor_get_status()
{
    int16_t res;
    uint16_t sensor_val;

    res = ADC_convert(sensor_adc, &sensor_val);
    if (res == ADC_STATUS_SUCCESS) {
        if (sensor_val > sensor_val_ref)
        {
            return true;
        }
    }
    return false;
}
