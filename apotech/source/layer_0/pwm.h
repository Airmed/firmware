
#ifndef __PWM_H__
#define __PWM_H__

#include <stdint.h>

void pwm_init();

void pwm_buzzer_set_freq(uint32_t freq);

#endif
