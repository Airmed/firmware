
#include "database.h"

#include "network.h"
#include "uart_term.h"

#include <stdint.h>

#define DATABASE_HOSTNAME ("https://seniordesigndb.herokuapp.com")

static const char database_query_medications[] = "{\"query\":\"select * from medications;\"}";
static const char database_query_schedule[] = "{\"query\":\"select * from schedule;\"}";

uint8_t database_read_medications(database_medication_t ** ptr_medication_arr)
{
    uint32_t num_medications = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, database_query_medications, &response);
    network_server_disconnect(handle);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    //*ptr_medication_arr =

    free(response);

    return num_medications;
}

uint8_t database_read_schedule(database_schedule_t ** ptr_schedule_arr)
{
    uint32_t num_schedule = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, database_query_schedule, &response);
    network_server_disconnect(handle);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    //*ptr_schedule_arr =

    free(response);

    return num_schedule;
}

void database_write_log(database_log_t log)
{

}
