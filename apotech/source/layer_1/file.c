
#include "uart_term.h"

#include "file.h"

#include <stdint.h>
#include <ti/drivers/net/wifi/fs.h>
#include <ti/drivers/net/wifi/simplelink.h>

static const char configuration_file_name[] = "configuration.txt";

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

