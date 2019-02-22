
#include "gpio.h"

#include "pin_map.h"

#include "board.h"
#include <stdbool.h>
#include "ti/drivers/GPIO.h"

void leds_init()
{
    GPIO_write(BOARD_GPIO_LED_STATUS, BOARD_PIN_OFF);
    GPIO_write(BOARD_GPIO_LED_ERROR, BOARD_PIN_OFF);
}

void buttons_init()
{

}

void led_status_off()
{
    GPIO_write(BOARD_GPIO_LED_STATUS, BOARD_PIN_OFF);
}

void led_status_on()
{
    GPIO_write(BOARD_GPIO_LED_STATUS, BOARD_PIN_ON);
}

void led_error_off()
{
    GPIO_write(BOARD_GPIO_LED_ERROR, BOARD_PIN_OFF);
}

void led_error_on()
{
    GPIO_write(BOARD_GPIO_LED_ERROR, BOARD_PIN_ON);
}

bool button_patient_get_status()
{

}

bool button_reset_get_status()
{

}
