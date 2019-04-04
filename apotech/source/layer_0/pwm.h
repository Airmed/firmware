
#ifndef __PWM_H__
#define __PWM_H__

#include "board.h"

#include <stdint.h>
#include <ti/drivers/PWM.h>

PWM_Handle pwm_init(board_pwm_name pwm_name);

void pwm_set_freq(PWM_Handle handle, uint32_t freq);
void pwm_off(PWM_Handle handle);

#endif
