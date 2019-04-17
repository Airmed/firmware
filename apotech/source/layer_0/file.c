
#include "uart_term.h"

#include "data_types.h"
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
                           SL_FS_CREATE | SL_FS_CREATE_MAX_SIZE( sizeof(configuration_t) ),
                           NULL);
        sl_FsClose(handle, NULL, NULL, 0);
    }
}

configuration_t file_configuration_read()
{
    configuration_t config;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, 0, (uint8_t *)&config, sizeof(configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);

    return config;
}

void file_configuration_write(configuration_t data)
{
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, 0, (uint8_t *)&data, sizeof(configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);
}

void file_configuration_print(configuration_t data)
{
    UART_PRINT("\n\r{\n\r");
    for (uint8_t i = 0; i < NUM_MEDICATIONS; i++)
    {
        medication_t * ptr_medication = &data.medication[i];
        UART_PRINT("  {%s, %d, {", ptr_medication->name, ptr_medication->count);
        for (uint8_t j = 0; j < NUM_DISPENSE_SLOTS; j++)
        {
            UART_PRINT(" {%d, %d, %d} ", ptr_medication->dispense_slot[j].valid, ptr_medication->dispense_slot[j].time_of_day, ptr_medication->dispense_slot[j].count);
        }
        UART_PRINT("}\n\r");
    }
    UART_PRINT("}\n\r\n\r");
}

#define MAX_NUM_LOGS (256)
#define FILE_LOG_COUNT_LOC (0)
#define FILE_LOG_LAST_READ_LOC (1)
#define FILE_LOG_LOC (2)

void file_log_init()
{
    int32_t handle;
    SlFsFileInfo_t info;

    char temp[(sizeof(log_t) * MAX_NUM_LOGS) + 2];
    memset(temp, 0, (sizeof(log_t) * MAX_NUM_LOGS) + 2);

//    if (sl_FsGetInfo((uint8_t *)log_file_name, 0, &info) != 0)
//    {
        handle = sl_FsOpen((uint8_t *)log_file_name,
                           SL_FS_CREATE | SL_FS_OVERWRITE | SL_FS_CREATE_MAX_SIZE( (sizeof(log_t) * MAX_NUM_LOGS) + 2 ),
                           NULL);

        int32_t ret = sl_FsWrite(handle, 0, (uint8_t *)&temp, (sizeof(log_t) * MAX_NUM_LOGS) + 2);
        sl_FsClose(handle, NULL, NULL, 0);


//    }
}

uint8_t file_log_read(log_t ** ptr_log_arr)
{
    uint8_t log_count, last_read;
    char file[(sizeof(log_t) * MAX_NUM_LOGS) + 2];

    int32_t handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, FILE_LOG_COUNT_LOC, (uint8_t *)file, (sizeof(log_t) * MAX_NUM_LOGS) + 2);

    log_count = file[FILE_LOG_COUNT_LOC];
    last_read = file[FILE_LOG_LAST_READ_LOC];

    uint8_t arr_len = log_count - last_read;
    if (arr_len <= 0)
    {
        sl_FsClose(handle, NULL, NULL, 0);
        return arr_len;
    }

    *ptr_log_arr = (log_t *)malloc(arr_len * sizeof(log_t));
    if (*ptr_log_arr == NULL)
    {
        UART_PRINT("malloc failed");
        sl_FsClose(handle, NULL, NULL, 0);
        return 0;
    }

    for (uint8_t i = 0; i < arr_len; i++)
    {
        log_t log;
        sl_FsRead(handle, FILE_LOG_LOC + (last_read + i) * sizeof(log_t), (uint8_t *)&log, sizeof(log_t));
        (*ptr_log_arr)[i] = log;
    }

    sl_FsClose(handle, NULL, NULL, 0);

    file[FILE_LOG_LAST_READ_LOC] = log_count;

    handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, 0, (uint8_t *)file, (sizeof(log_t) * MAX_NUM_LOGS) + 2);
    sl_FsClose(handle, NULL, NULL, 0);

    return arr_len;
}

void file_log_write(log_t log)
{
    uint8_t log_count;
    int32_t handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_READ, NULL);

    char file[(sizeof(log_t) * MAX_NUM_LOGS) + 2];

    sl_FsRead(handle, 0, (uint8_t *)file, (sizeof(log_t) * MAX_NUM_LOGS) + 2);
    sl_FsClose(handle, NULL, NULL, 0);

    log_count = file[FILE_LOG_COUNT_LOC];
    file[FILE_LOG_COUNT_LOC] = log_count + 1;

    memcpy(&file[FILE_LOG_LOC + log_count * sizeof(log_t)], &log, sizeof(log_t));

    handle = sl_FsOpen((uint8_t *)log_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, 0, (uint8_t *)&file, (sizeof(log_t) * MAX_NUM_LOGS) + 2);
    sl_FsClose(handle, NULL, NULL, 0);
}
