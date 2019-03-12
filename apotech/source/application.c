
#include "buzzer.h"
#include "drum.h"
#include "gpio.h"
#include "flash.h"
#include "sensor.h"
#include "shutter.h"

#include "board.h"
#include <stdbool.h>
#include <unistd.h>
#include "ti/drivers/GPIO.h"

void hardware_init();

void * application_thread(void * arg0)
{
    hardware_init();

    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}

void hardware_init()
{
    shutter_init();
    drum_init();

    buzzer_init();
    flash_init();

    leds_init();
    buttons_init();
}
