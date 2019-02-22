
#include "gpio.h"

#include "pin_map.h"

#include "board.h"
#include <stdbool.h>
#include "ti/drivers/GPIO.h"

void sensor_init()
{

}

bool sensor_get_status()
{
    return GPIO_read(BOARD_SENSOR);
}
