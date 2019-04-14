
#include "board.h"
#include "buttons.h"
#include "buzzer.h"
#include "database.h"
#include "drum.h"
#include "file.h"
#include "sensor.h"
#include "shutter.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ti/drivers/GPIO.h>
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

void poll_database()
{
    database_medication_t * medication_arr = NULL;
    database_schedule_t * schedule_arr = NULL;

    uint8_t medication_count = database_read_medications(&medication_arr);
    uint8_t schedule_count = database_read_schedule(&schedule_arr);

    file_configuration_t configuration;
    memset(&configuration, 0, sizeof(file_configuration_t));

    // TODO translate

    file_configuration_write(configuration);

    free((void *)medication_arr);
    free((void *)schedule_arr);
}
