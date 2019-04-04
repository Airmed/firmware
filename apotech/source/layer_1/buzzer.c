
#include "buzzer.h"

#include "board.h"
#include "pwm.h"

#include <stdint.h>

static PWM_Handle buzzer_pwm_handle;

void buzzer_init()
{
    buzzer_pwm_handle = pwm_init(BOARD_BUZZER_PWM);
}

void buzzer_set_freq(uint32_t freq)
{
    pwm_set_freq(buzzer_pwm_handle, freq);
}

void buzzer_off()
{
    pwm_off(buzzer_pwm_handle);
}
