
#include "buttons.h"

#include "board.h"
#include "pin_map.h"

#include <stdbool.h>
#include <ti/drivers/GPIO.h>

bool button_patient_get_status()
{
    bool temp = GPIO_read(BOARD_GPIO_BUTTON_PATIENT);
    return temp;
}

bool button_reset_get_status()
{
    return GPIO_read(BOARD_GPIO_BUTTON_RESET);
}
