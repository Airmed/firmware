
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdint.h>

typedef enum
{
    BUTTON_INTERRUPT_DISPENSE,
    BUTTON_INTERRUPT_SILENCE
} button_interrupt_e;

void init_button_interrupts();
void dispense();
void patient_button_callback(uint_least8_t index);

#endif
