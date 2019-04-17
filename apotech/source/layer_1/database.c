
#include "database.h"

#include "data_types.h"
#include "network.h"
#include "uart_term.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/utils/json/json.h>

#define DATABASE_HOSTNAME ("https://seniordesigndb.herokuapp.com")
#define DATABASE_MAX_COMMAND_LEN (128)
#define DATABASE_TIMESTAMP_LEN (27)
#define DATABASE_MAX_TYPE_STR_LEN (32)

static const char str_query_medications[] = "{\"query\":\"SELECT * FROM medications;\"}";
static const char str_update_medication_qty[] = "{\"query\":\"UPDATE medications SET qty=%d WHERE id=%d;\"}";
static const char str_query_schedule[] = "{\"query\":\"SELECT * FROM schedule;\"}";
static const char str_add_log[] = "{\"query\":\"INSERT INTO history (cur_time, error) VALUES ('%s', '%s');\"}";
static const char str_add_notification[] = "{\"query\":\"INSERT INTO notifications (cur_time, error) VALUES ('%s', '%s');\"}";

static const char str_medications_template[] = "{\"success\":boolean,\"data\":[{\"id\":uint32,\"name\":string,\"qty\":uint32}]}";
static const char str_schedule_template[] = "{\"success\":boolean,\"data\":[{\"id\":uint32,\"med_id\":uint32,\"hour\":uint32,\"minute\":uint32,\"num_disp\":uint32}]}";

typedef enum
{
    DATABASE_LOG_TYPE_DISPENSE_ERROR,
    DATABASE_LOG_TYPE_DISPENSE_SUCCESS,
    DATABASE_LOG_TYPE_PILLS_TAKEN
} database_log_type_t;

static void database_read_medications(configuration_t * ptr_configuration)
{
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_medications, &response);
    network_server_disconnect(handle);

    if (response_len > 0)
    {
        for (uint32_t i = 1; i < strlen(response); i++)
        {
            if (response[i-1] == ']' && response[i] == '}')
            {
                response[i+1] = '\0';
                break;
            }
        }
        UART_PRINT("\n\r%s\n\r", response);

        Json_Handle template_handle;
        Json_Handle object_handle, object_bak;
        bool success;
        int32_t ret;

        ret = Json_createTemplate(&template_handle, str_medications_template, strlen(str_medications_template));
        ret |= Json_createObject(&object_handle, template_handle, 0);
        ret |= Json_parse(object_handle, response, strlen(response));
        object_bak = object_handle;

        ret |= Json_getValue(object_handle, "\"success\"", &success, NULL);

        if (success == true)
        {
            int32_t num_medications = Json_getArrayMembersCount(object_bak, "\"data\"");
            for (uint8_t i = 0; i < num_medications && i < NUM_MEDICATIONS; i++)
            {
                uint32_t id;
                char key[32];

                sprintf(key, "\"data\"[%d]\"id\"", i);
                ret = Json_getValue(object_bak, key, &id, NULL);
                id--;

                sprintf(key, "\"data\"[%d]\"name\"", i);
                ret = Json_getValue(object_bak, key, ptr_configuration->medication[id].name, NULL);

                sprintf(key, "\"data\"[%d]\"qty\"", i);
                ret = Json_getValue(object_bak, key, &ptr_configuration->medication[id].count, NULL);
            }
        }

        ret = Json_destroyObject(object_bak);
        ret = Json_destroyTemplate(template_handle);
    }

    free(response);
}

static void database_read_schedule(configuration_t * ptr_configuration)
{
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_schedule, &response);
    network_server_disconnect(handle);

    if (response_len > 0)
    {
        for (uint32_t i = 1; i < strlen(response); i++)
        {
            if (response[i-1] == ']' && response[i] == '}')
            {
                response[i+1] = '\0';
                break;
            }
        }
        UART_PRINT("\n\r%s\n\r", response);

        Json_Handle template_handle;
        Json_Handle object_handle, object_bak;
        bool success;
        int32_t ret;

        ret = Json_createTemplate(&template_handle, str_schedule_template, strlen(str_schedule_template));
        ret |= Json_createObject(&object_handle, template_handle, 0);
        ret |= Json_parse(object_handle, response, strlen(response));
        object_bak = object_handle;

        ret |= Json_getValue(object_handle, "\"success\"", &success, NULL);

        if (success == true)
        {
            int32_t num_slots = Json_getArrayMembersCount(object_bak, "\"data\"");
            for (uint8_t i = 0; i < num_slots; i++)
            {
                uint32_t id, idx = 0;
                uint32_t hour, minute;
                char key[32];

                sprintf(key, "\"data\"[%d]\"med_id\"", i);
                ret = Json_getValue(object_bak, key, &id, NULL);
                id--;

                dispense_slot_t * ptr_slot = &ptr_configuration->medication[id].dispense_slot[idx];
                while (ptr_slot->valid == true)
                {
                    idx++;
                    ptr_slot = &ptr_configuration->medication[id].dispense_slot[idx];
                }
                if (idx >= NUM_DISPENSE_SLOTS) continue;

                ptr_slot->valid = true;

                sprintf(key, "\"data\"[%d]\"hour\"", i);
                ret = Json_getValue(object_bak, key, &hour, NULL);
                sprintf(key, "\"data\"[%d]\"minute\"", i);
                ret = Json_getValue(object_bak, key, &minute, NULL);
                ptr_slot->time_of_day = (hour * 60 * 60) + (minute * 60);

                sprintf(key, "\"data\"[%d]\"num_disp\"", i);
                ret = Json_getValue(object_bak, key, &ptr_slot->count, NULL);
            }
        }

        ret = Json_destroyObject(object_bak);
        ret = Json_destroyTemplate(template_handle);
    }

    free(response);
}

configuration_t database_read_configuration()
{
    configuration_t configuration;
    memset(&configuration, 0, sizeof(configuration_t));

    database_read_medications(&configuration);
    database_read_schedule(&configuration);

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

#define SECONDS_UNTIL_APR_1 (3763065600)

void database_write_log(log_t log, bool notify)
{
    char command[DATABASE_MAX_COMMAND_LEN];
    char type_str[DATABASE_MAX_TYPE_STR_LEN];
    char timestamp_str[DATABASE_TIMESTAMP_LEN];
    char * response;

    uint8_t day = (log.time - SECONDS_UNTIL_APR_1) / (24 * 60 * 60) + 1;
    uint8_t hour = (log.time % (24 * 60 * 60)) / (60 * 60);
    uint8_t minute = (log.time % (60 * 60)) / 60;
    uint8_t second = log.time % 60;
    sprintf(timestamp_str, "2019-04-%02d %02d:%02d:%02d.000000", day, hour, minute, second);

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
        default:
            strcpy(type_str, "Unknown log type");
            break;
    }

    sprintf(command, str_add_log, timestamp_str, type_str);
    UART_PRINT("\n\r%s\n\r\n\r", command);

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    network_send_request(handle, command, &response);
    network_server_disconnect(handle);

    free(response);

    if (notify == true)
    {
        sprintf(command, str_add_notification, timestamp_str, type_str);
        UART_PRINT("\n\r%s\n\r\n\r", command);

        network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
        network_send_request(handle, command, &response);
        network_server_disconnect(handle);

        free(response);
    }
}
