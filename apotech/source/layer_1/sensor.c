
#include "sensor.h"

#include "board.h"
#include "pin_map.h"

#include <stdbool.h>
#include <stdint.h>
#include "ti/drivers/ADC.h"

void sensor_init(sensor_t * sensor)
{
    ADC_Params params;

    ADC_init();
    ADC_Params_init(&params);

    sensor->adc_handle = ADC_open(sensor->adc, &params);
    if (sensor->adc_handle == 0) while (1);

    ADC_convert(sensor->adc_handle, &sensor->reference_val);
    sensor->reference_val *= 1.5f;
}

bool sensor_get_status(sensor_t * sensor)
{
    int16_t res;
    uint16_t sensor_val;

    res = ADC_convert(sensor->adc_handle, &sensor_val);
    if (res == ADC_STATUS_SUCCESS)
    {
        if (sensor_val > sensor->reference_val)
        {
            return true;
        }
    }
    return false;
}
