
#include "pwm.h"

#include "board.h"
#include "pin_map.h"

#include <stddef.h>
#include <stdint.h>
#include <ti/drivers/PWM.h>

PWM_Handle pwm_init(board_pwm_name pwm_name)
{
    PWM_init();
    PWM_Handle handle = PWM_open(pwm_name, NULL);
    if (handle == 0)
    {
        while(1);
    }

    return handle;
}

#define PWM_DUTY_CYCLE_50 (0x80000000)

void pwm_set_freq(PWM_Handle handle, uint32_t freq)
{
    PWM_setDutyAndPeriod(handle, PWM_DUTY_CYCLE_50, freq);
}

void pwm_off(PWM_Handle handle)
{
    PWM_stop(handle);
}
