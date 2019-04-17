
#include "board.h"
#include "events.h"
#include "log.h"
#include "uart_term.h"

#include <stdbool.h>
#include <ti/drivers/GPIO.h>
#include <unistd.h>

extern uint32_t unhandled_events;

void patient_button_callback_at_04(uint_least8_t index);
extern void init_updates();

void * at_04_thread(void * arg0)
{
    init_updates();

    GPIO_setCallback(BOARD_GPIO_BUTTON_PATIENT, patient_button_callback_at_04);
    GPIO_enableInt(BOARD_GPIO_BUTTON_PATIENT);

    while (1)
    {
        uint32_t prev_events = unhandled_events;
        unhandled_events &= ~prev_events;

        if (prev_events & EVENT_UPDATE)
        {
            UART_PRINT("checking logs\n\r");
            log_send_new();
        }

        if (prev_events & EVENT_DISPENSE)
        {
            UART_PRINT("log added\n\r");
            log_new(LOG_TYPE_DISPENSE_FAILED);
        }

        usleep(1000);
    }

    return 0;
}

void patient_button_callback_at_04(uint_least8_t index)
{
    unhandled_events |= EVENT_DISPENSE;
}
