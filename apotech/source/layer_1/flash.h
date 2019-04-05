
#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

#define MAX_MEDICATIONS (4)
#define MAX_MEDICATION_NAME_LEN (32)
#define MAX_DISPENSE_SLOTS (3)

typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t count;
} dispense_slot_t;

typedef struct
{
    char name[MAX_MEDICATION_NAME_LEN];
    uint8_t count;
    dispense_slot_t dispense_slot[MAX_DISPENSE_SLOTS];
} medication_t;

typedef struct
{
    medication_t medication[MAX_MEDICATIONS];
} configuration_t;

void configuration_init();
configuration_t configuration_read();
void configuration_write(configuration_t data);

#endif
