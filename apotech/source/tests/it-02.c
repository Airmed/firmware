
#include "board.h"
#include "buttons.h"
#include "shutter.h"

#include <pthread.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>

void * it_02_thread(void * arg0)
{
    while (true)
    {
        while (button_patient_get_status() == false);

        shutter_dispense();
    }

    return 0;
}
