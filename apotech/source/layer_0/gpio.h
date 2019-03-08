
#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdbool.h>

void leds_init();
void buttons_init();

void led_status_off();
void led_status_on();

void led_error_off();
void led_error_on();

bool button_patient_get_status();

bool button_reset_get_status();

#endif
