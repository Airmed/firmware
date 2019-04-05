
#include "flash.h"

#include <stdint.h>
#include <ti/drivers/net/wifi/fs.h>
#include <ti/drivers/net/wifi/simplelink.h>

static const char configuration_file_name[] = "configuration.txt";

void configuration_init()
{
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name,
                               SL_FS_CREATE | SL_FS_CREATE_MAX_SIZE( sizeof(configuration_t) ),
                               NULL);
    sl_FsClose(handle, NULL, NULL, 0);
}

configuration_t configuration_read()
{
    configuration_t ret;
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_READ, NULL);
    sl_FsRead(handle, 0, (uint8_t *)&ret, sizeof(configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);

    return ret;
}

void configuration_write(configuration_t data)
{
    int32_t handle = sl_FsOpen((uint8_t *)configuration_file_name, SL_FS_WRITE, NULL);
    sl_FsWrite(handle, 0, (uint8_t *)&data, sizeof(configuration_t));
    sl_FsClose(handle, NULL, NULL, 0);
}
