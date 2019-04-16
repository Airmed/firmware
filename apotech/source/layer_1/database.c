
#include "database.h"

#include "data_types.h"
#include "network.h"
#include "uart_term.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DATABASE_HOSTNAME ("https://seniordesigndb.herokuapp.com")
#define DATABASE_MAX_COMMAND_LEN (128)
#define DATABASE_TIMESTAMP_LEN (27)
#define DATABASE_MAX_TYPE_STR_LEN (32)

static const char str_query_medications[] = "{\"query\":\"SELECT * FROM medications;\"}";
static const char str_update_medication_qty[] = "{\"query\":\"UPDATE medications SET qty=%d WHERE id=%d;\"}";
static const char str_query_schedule[] = "{\"query\":\"SELECT * FROM schedule;\"}";
static const char str_add_log[] = "{\"query\":\"INSERT INTO history (cur_time, error) VALUES (%lu, %s);\"}";
static const char str_add_notification[] = "{\"query\":\"INSERT INTO notifications (cur_time, error) VALUES (%s, %s);\"}";

typedef struct
{
    uint32_t temp;
} database_schedule_t;

typedef struct
{
    uint32_t temp;
} database_medication_t;

typedef enum
{
    DATABASE_LOG_TYPE_DISPENSE_ERROR,
    DATABASE_LOG_TYPE_DISPENSE_SUCCESS,
    DATABASE_LOG_TYPE_PILLS_TAKEN
} database_log_type_t;

static uint8_t database_read_medications(database_medication_t ** ptr_medication_arr)
{
    uint32_t num_medications = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_medications, &response);
    network_server_disconnect(handle);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    //*ptr_medication_arr =

    free(response);

    return num_medications;
}

static uint8_t database_read_schedule(database_schedule_t ** ptr_schedule_arr)
{
    uint32_t num_schedule = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_schedule, &response);
    network_server_disconnect(handle);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    //*ptr_schedule_arr =

    free(response);

    return num_schedule;
}

configuration_t database_read_configuration()
{
    database_medication_t * medication_arr;
    database_schedule_t * schedue_arr;

    uint8_t num_medications = database_read_medications(&medication_arr);
    uint8_t num_schedule = database_read_schedule(&schedue_arr);

    configuration_t configuration;
    memset(&configuration, 0, sizeof(configuration_t));

    // TODO translate

//    free(medication_arr);
//    free(schedue_arr);

    return configuration;
}

void database_write_medication_qty(uint8_t med_id, uint8_t qty)
{
    char command[DATABASE_MAX_COMMAND_LEN];
    char * response;

    sprintf(command, str_update_medication_qty, qty, med_id);

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    network_send_request(handle, command, &response);
    network_server_disconnect(handle);

    free(response);
}

void database_write_log(log_t log, bool notify)
{
    char command[DATABASE_MAX_COMMAND_LEN];
    char type_str[DATABASE_MAX_TYPE_STR_LEN];
    char timestamp_str[DATABASE_TIMESTAMP_LEN];
    char * response;

    // TODO time handling
//    sprintf(timestamp_str, "%04d-%02d-%02d %02d:%02d:%02d.000000", log.year, log.month, log.day, log.hour, log.minute, log.second);

    switch (log.type)
    {
        case DATABASE_LOG_TYPE_DISPENSE_ERROR:
            strcpy(type_str, "Dispense Error");
            break;
        case DATABASE_LOG_TYPE_DISPENSE_SUCCESS:
            strcpy(type_str, "Dispense Success");
            break;
        case DATABASE_LOG_TYPE_PILLS_TAKEN:
            strcpy(type_str, "Pills Taken");
            break;
    }

    sprintf(command, str_add_log, log.time, type_str);

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    network_send_request(handle, command, &response);
    network_server_disconnect(handle);

    free(response);

    if (notify == true)
    {
        sprintf(command, str_add_notification, timestamp_str, type_str);

        network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
        network_send_request(handle, command, &response);
        network_server_disconnect(handle);

        free(response);
    }
}
