
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <ti/drivers/net/wifi/simplelink.h>

#define NETWORK_SSID ("airmed")
#define NETWORK_SECURITY (SL_WLAN_SEC_TYPE_WPA_WPA2)
#define NETWORK_PASSWORD ("apotech!")

void network_connect();

#endif
