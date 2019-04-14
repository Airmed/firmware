
#ifndef __FILE_H__
#define __FILE_H__

#include <stdbool.h>
#include <stdint.h>

#define FILE_MAX_MEDICATIONS (4)
#define FILE_MAX_MEDICATION_NAME_LEN (32)
#define FILE_MAX_DISPENSE_SLOTS (3)

typedef struct
{
    uint8_t time_of_day;
    uint8_t count;
} file_dispense_slot_t;

typedef struct
{
    char name[FILE_MAX_MEDICATION_NAME_LEN];
    uint8_t count;
    bool next;
    file_dispense_slot_t dispense_slot[FILE_MAX_DISPENSE_SLOTS];
} file_medication_t;

typedef struct
{
    file_medication_t medication[FILE_MAX_MEDICATIONS];
} file_configuration_t;

void file_configuration_init();
file_configuration_t file_configuration_read();
void file_configuration_write(file_configuration_t data);
void file_configuration_print(file_configuration_t data);

typedef struct
{
    uint8_t type;
    uint32_t time;
    bool notify;
} file_log_header_t;

typedef struct
{
    file_log_header_t header;
} file_log_t;

void file_log_init();
uint8_t file_log_read(file_log_t ** ptr_log_arr);
void file_log_write(file_log_t log);

#endif
