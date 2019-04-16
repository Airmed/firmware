
#include "events.h"
#include "file.h"
#include "schedule.h"

#include <unistd.h>

//typedef enum
//{
//    NETWORK_STATUS_READY,
//    NETWORK_STATUS_CONNECTED,
//    NETWORK_STATUS_DISCONNECTED
//} network_status_e;

extern uint32_t unhandled_events;
//extern volatile network_status_e status;

extern void init_updates();

void * at_02_thread(void * arg0)
{
    init_updates();

    while (1)
    {
        uint32_t prev_events = unhandled_events;
        unhandled_events &= ~prev_events;

        if (prev_events & EVENT_UPDATE)
        {
//            if (status == NETWORK_STATUS_READY)
//            {
                schedule_update();
//            }

            configuration_t configuration = file_configuration_read();
            file_configuration_print(configuration);
        }

        usleep(1000);
    }

    return 0;
}
