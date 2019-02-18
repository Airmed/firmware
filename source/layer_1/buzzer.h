
#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <stdint.h>

void buzzer_init();

void buzzer_set_freq(uint32_t freq);
void buzzer_off();

#endif
