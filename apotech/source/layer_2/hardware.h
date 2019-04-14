
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdint.h>

void init_button_interrupts();
void dispense();
void patient_button_callback(uint_least8_t index);

#endif
