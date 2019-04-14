
#include "uart_term.h"

#include "file.h"

#include <stdint.h>
#include <ti/drivers/net/wifi/fs.h>
#include <ti/drivers/net/wifi/simplelink.h>

static const char configuration_file_name[] = "configuration.txt";
static const char log_file_name[] = "log.txt";

void file_configuration_init()
{
    int32_t handle;
    SlFsFileInfo_t info;

    if (sl_FsGetInfo((uint8_t *)configuration_file_name, 0, &info) != 0)
    {
        handle = sl_FsOpen((uint8_t *)configuration_file_name,
                           SL_FS_CREATE | SL_FS_CREATE_MAX_SIZE( sizeof(file_configuration_t) ),
                           NULL);
        sl_FsClose(handle, NULL, NULL, 0);
    }
}

file_configuration_t file_configuration_read()
{
    file_configuration_t config;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, 0, (uint8_t *)&config, sizeof(file_configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);

    return config;
}

void file_configuration_write(file_configuration_t data)
{
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, 0, (uint8_t *)&data, sizeof(file_configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);
}

void file_configuration_print(file_configuration_t data)
{
    UART_PRINT("{\r\n");
    for (uint8_t i = 0; i < FILE_MAX_MEDICATIONS; i++)
    {
        file_medication_t * ptr_medication = &data.medication[i];
        UART_PRINT("  {%s, %d, {", ptr_medication->name, ptr_medication->count);
        for (uint8_t j = 0; j < FILE_MAX_DISPENSE_SLOTS; j++)
        {
            UART_PRINT(" {%d, %d, %d} ", ptr_medication->dispense_slot[j].time_of_day, ptr_medication->dispense_slot[j].count);
        }
        UART_PRINT("}\r\n");
    }
    UART_PRINT("}\r\n");
}

#define MAX_NUM_LOGS (256)
#define FILE_LOG_COUNT_LOC (0)
#define FILE_LOG_LAST_READ_LOC (1)
#define FILE_LOG_LOC (2)

void file_log_init()
{
    int32_t handle;
    SlFsFileInfo_t info;

    if (sl_FsGetInfo((uint8_t *)log_file_name, 0, &info) != 0)
    {
        handle = sl_FsOpen((uint8_t *)log_file_name,
                           SL_FS_CREATE | SL_FS_CREATE_MAX_SIZE( sizeof(file_log_t) * MAX_NUM_LOGS + 2 ),
                           NULL);
        sl_FsClose(handle, NULL, NULL, 0);
    }
}

uint8_t file_log_read(file_log_t ** ptr_log_arr)
{
    uint8_t log_count, last_read;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, FILE_LOG_COUNT_LOC, (uint8_t *)&log_count, 1);
    sl_FsRead(handle, FILE_LOG_LAST_READ_LOC, (uint8_t *)&last_read, 1);

    uint8_t arr_len = log_count - last_read;
    *ptr_log_arr = (file_log_t *)malloc(arr_len * sizeof(file_log_t));
    if (*ptr_log_arr == NULL)
    {
        UART_PRINT("malloc failed");
        return 0;
    }

    for (uint8_t i = 0; i < arr_len; i++)
    {
        file_log_t log;
        sl_FsRead(handle, FILE_LOG_COUNT_LOC + (last_read + i + 1) * sizeof(file_log_t), (uint8_t *)&log, sizeof(file_log_t));
        (*ptr_log_arr)[i] = log;
    }

    sl_FsClose(handle, NULL, NULL, 0);
    handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, FILE_LOG_LAST_READ_LOC, (uint8_t *)&last_read, 1);
    sl_FsClose(handle, NULL, NULL, 0);

    return arr_len;
}

void file_log_write(file_log_t log)
{
    uint8_t log_count;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, FILE_LOG_COUNT_LOC, (uint8_t *)&log_count, 1);
    sl_FsClose(handle, NULL, NULL, 0);

    log_count++;

    handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, FILE_LOG_COUNT_LOC, (uint8_t *)&log_count, 1);
    sl_FsWrite(handle, FILE_LOG_LOC + log_count * sizeof(file_log_t), (uint8_t *)&log, sizeof(file_log_t));
    sl_FsClose(handle, NULL, NULL, 0);
}
