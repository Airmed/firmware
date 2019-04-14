
#include "database.h"

#include "network.h"
#include "uart_term.h"

#include <stdint.h>

#define DATABASE_HOSTNAME ("https://seniordesigndb.herokuapp.com")
#define DATABASE_QUERY_MEDICATIONS ("{\"query\":\"select * from medications;\"}")
#define DATABASE_QUERY_SCHEDULE ("{\"query\":\"select * from schedule;\"}")

static network_handle_t handle;

void database_init()
{
    network_connect();
    handle = network_server_connect(DATABASE_HOSTNAME);
}

void database_deinit()
{
    network_server_disconnect(handle);
    network_disconnect();
}

uint8_t database_read_medications(database_medication_t ** ptr_medication_arr)
{
    uint32_t num_medications = 0;
    char * response;

    uint32_t response_len = network_send_request(handle, DATABASE_QUERY_MEDICATIONS, &response);

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

    uint32_t response_len = network_send_request(handle, DATABASE_QUERY_SCHEDULE, &response);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    //*ptr_schedule_arr =

    free(response);

    return num_schedule;
}

void database_write_log(database_log_t log)
{

}
