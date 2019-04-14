
#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "drum.h"
#include "flash.h"
#include "sensor.h"
#include "shutter.h"

#include <stdbool.h>
#include "ti/drivers/GPIO.h"
#include <unistd.h>

void * prod_thread(void * arg0)
{
    while (true)
    {
        while (button_patient_get_status() == false) usleep(1000);

        shutter_dispense();
    }

    return 0;
}
