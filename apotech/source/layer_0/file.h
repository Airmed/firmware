
#ifndef __FILE_H__
#define __FILE_H__

#include "data_types.h"

#include <stdbool.h>
#include <stdint.h>

void file_configuration_init();
configuration_t file_configuration_read();
void file_configuration_write(configuration_t data);
void file_configuration_print(configuration_t data);

void file_log_init();
uint8_t file_log_read(log_t ** ptr_log_arr);
void file_log_write(log_t log);

#endif
