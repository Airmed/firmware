
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>

#define MAX_MEDICATION_NAME_LEN (32)
#define MAX_DISPENSE_SLOTS (3)

typedef struct
{
    uint32_t temp;
} database_schedule_t;

typedef struct
{
    uint32_t temp;
} database_medication_t;

typedef struct
{
    uint32_t temp;
} database_log_t;

uint8_t database_read_medications(database_medication_t ** ptr_medication_arr);
uint8_t database_read_schedule(database_schedule_t ** ptr_schedule_arr);
void database_write_log(database_log_t log);

#endif
