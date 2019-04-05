
#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "drum.h"
#include "sensor.h"
#include "shutter.h"

#include <stdbool.h>
#include "ti/drivers/GPIO.h"
#include <unistd.h>

void * application_thread(void * arg0)
{
    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}
