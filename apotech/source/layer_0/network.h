
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/http/httpclient.h>

#define NETWORK_SSID ("UCB Wireless")
#define NETWORK_SECURITY (SL_WLAN_SEC_TYPE_OPEN)
#define NETWORK_PASSWORD ("")

typedef HTTPClient_Handle network_handle_t;

void network_connect();
void network_disconnect();

network_handle_t network_server_connect(const char * hostname);
void network_server_disconnect(const network_handle_t handle);
uint32_t network_send_request(const network_handle_t handle, const char * command, char ** ptr_response);

#endif
