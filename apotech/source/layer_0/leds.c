
#include "leds.h"

#include "board.h"
#include "pin_map.h"

#include <ti/drivers/GPIO.h>

void leds_init()
{
    GPIO_write(BOARD_GPIO_LED_STATUS, BOARD_PIN_OFF);
    GPIO_write(BOARD_GPIO_LED_ERROR, BOARD_PIN_OFF);
    GPIO_write(BOARD_GPIO_LED_CONNECTION, BOARD_PIN_OFF);
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

void led_connection_off()
{
    GPIO_write(BOARD_GPIO_LED_CONNECTION, BOARD_PIN_OFF);
}

void led_connection_on()
{
    GPIO_write(BOARD_GPIO_LED_CONNECTION, BOARD_PIN_ON);
}
