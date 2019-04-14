
#include "buzzer.h"

#include "board.h"

#include <stddef.h>
#include <stdint.h>
#include <ti/drivers/PWM.h>

static PWM_Handle handle;

void buzzer_init()
{
    PWM_init();
    PWM_Handle handle = PWM_open(BOARD_BUZZER_PWM, NULL);
    if (handle == 0)
    {
        while(1);
    }
}

#define PWM_DUTY_CYCLE_50 (0x80000000)

void buzzer_set_freq(uint32_t freq)
{
    PWM_setDutyAndPeriod(handle, PWM_DUTY_CYCLE_50, freq);
}

void buzzer_off()
{
    PWM_stop(handle);
}
