
#ifndef __SHUTTER_H__
#define __SHUTTER_H__

typedef enum
{
    SHUTTER_DISPENSE_SUCCESS,
    SHUTTER_DISPENSE_NONE,
    SHUTTER_DISPENSE_MULTIPLE
} shutter_dispense_t;

void shutter_init();

shutter_dispense_t shutter_dispense();

#endif
