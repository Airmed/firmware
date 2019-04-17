
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "data_types.h"


#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    char name[MAX_MEDICATION_NAME_LEN];
    uint8_t count;
} database_medication_t;

configuration_t database_read_configuration();
void database_write_medication_qty(uint8_t med_id, uint8_t qty);

void database_write_log(log_t log, bool notify);

void parse_schedule(char * response, dispense_slot_t **medicationPtr);
void parse_medication(char * response, database_medication_t **medicationPtr);

void getValue(char* key);


#endif
