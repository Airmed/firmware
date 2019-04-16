
#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include "stdbool.h"
#include "stdint.h"

#define NUM_MEDICATIONS (4)
#define NUM_DISPENSE_SLOTS (3)

#define MAX_MEDICATION_NAME_LEN (128)

typedef struct
{
    bool valid;
    uint32_t time_of_day;
    uint8_t count;
} dispense_slot_t;

typedef struct
{
    char name[MAX_MEDICATION_NAME_LEN];
    uint8_t count;
    dispense_slot_t dispense_slot[NUM_DISPENSE_SLOTS];
} medication_t;

typedef struct
{
    medication_t medication[NUM_MEDICATIONS];
} configuration_t;

typedef enum
{
    LOG_TYPE_DISPENSE_FAILED,
    LOG_TYPE_DISPENSE_SUCCESS,
    LOG_TYPE_PILLS_TAKEN
} log_type_e;

typedef struct
{
    log_type_e type;
    uint32_t time;
    bool notify;
} log_t;

typedef union
{
    uint32_t data;
    uint8_t medication[NUM_MEDICATIONS];
} dispense_counts_t;

#endif
