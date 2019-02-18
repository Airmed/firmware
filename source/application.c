
#include "buzzer.h"
#include "drum.h"
#include "gpio.h"
#include "flash.h"
#include "shutter.h"

void hardware_init();

void * application_thread(void * arg0)
{
    hardware_init();
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
