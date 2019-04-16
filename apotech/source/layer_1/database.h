
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "data_types.h"

#include <stdbool.h>
#include <stdint.h>

configuration_t database_read_configuration();
void database_write_medication_qty(uint8_t med_id, uint8_t qty);

void database_write_log(log_t log, bool notify);

#endif
