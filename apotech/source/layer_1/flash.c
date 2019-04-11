
#include "flash.h"

#include "uart_term.h"

#include <stdint.h>
#include <ti/drivers/net/wifi/fs.h>
#include <ti/drivers/net/wifi/simplelink.h>

static const char configuration_file_name[] = "configuration.txt";

void configuration_init()
{
    int32_t handle;
    SlFsFileInfo_t info;

    if (sl_FsGetInfo((uint8_t *)configuration_file_name, NULL, &info) != 0)
    {
        handle = sl_FsOpen((uint8_t *)configuration_file_name,
                           SL_FS_CREATE | SL_FS_CREATE_MAX_SIZE( sizeof(configuration_t) ),
                           NULL);
        sl_FsClose(handle, NULL, NULL, 0);
    }
}

configuration_t configuration_read()
{
    int32_t ret;
    configuration_t config;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    ret = sl_FsRead(handle, 0, (uint8_t *)&config, sizeof(configuration_t));
    ret = sl_FsClose(handle, NULL, NULL, 0);

    return config;
}

void configuration_write(configuration_t data)
{
    int32_t ret;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_WRITE, NULL);
    ret = sl_FsWrite(handle, 0, (uint8_t *)&data, sizeof(configuration_t));
    ret = sl_FsClose(handle, NULL, NULL, 0);
}

void configuration_print(configuration_t data)
{
    UART_PRINT("{\r\n");
    for (uint8_t i = 0; i < MAX_MEDICATIONS; i++)
    {
        medication_t * ptr_medication = &data.medication[i];
        UART_PRINT("  {%s, %d, {", ptr_medication->name, ptr_medication->count);
        for (uint8_t j = 0; j < MAX_DISPENSE_SLOTS; j++)
        {
            UART_PRINT(" {%d, %d, %d} ", ptr_medication->dispense_slot[j].hour, ptr_medication->dispense_slot[j].minute, ptr_medication->dispense_slot[j].count);
        }
        UART_PRINT("}\r\n");
    }
    UART_PRINT("}\r\n");
}

