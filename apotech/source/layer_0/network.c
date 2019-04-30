
#include "network.h"

#include "leds.h"
#include "uart_term.h"

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>
#include <ti/net/http/httpclient.h>
#include <unistd.h>

typedef enum
{
    NETWORK_STATUS_READY,
    NETWORK_STATUS_CONNECTED,
    NETWORK_STATUS_DISCONNECTED
} network_status_e;

volatile network_status_e status = NETWORK_STATUS_DISCONNECTED;

#define SL_STOP_TIMEOUT (200)
#define SLNET_IF_WIFI_NAME ("CC32xx")
#define SLNET_IF_WIFI_PRIO (5)

void network_connect()
{
    int32_t ret;
    SlWlanSecParams_t network_params;

    if (status != NETWORK_STATUS_READY)
    {
        ret = sl_WlanSetMode(ROLE_STA);

        ret = sl_Stop(SL_STOP_TIMEOUT);
        if(ret < 0)
        {
            UART_PRINT("sl_Stop failed\n\r");
            while(1);
        }

        ret = sl_Start(NULL, NULL, NULL);
        if(ret != ROLE_STA)
        {
            UART_PRINT("role is not ROLE_STA\n\r");
            while(1);
        }

        network_params.Type = NETWORK_SECURITY;
        network_params.Key = (int8_t *)NETWORK_PASSWORD;
        network_params.KeyLen = strlen( (char *)network_params.Key );

        ret = sl_WlanConnect((signed char *)NETWORK_SSID, strlen(NETWORK_SSID), NULL, &network_params, NULL);
        if (ret != 0)
        {
            UART_PRINT("failed to connect");
            while(1);
        }

        sleep(1);

        SlNetIf_init(0);
        SlNetIf_add(SLNETIF_ID_1, SLNET_IF_WIFI_NAME, (const SlNetIf_Config_t *)&SlNetIfConfigWifi, SLNET_IF_WIFI_PRIO);
        SlNetSock_init(0);
        SlNetUtil_init(0);
    }

    uint8_t disconnected_count = 0;
    while (status != NETWORK_STATUS_READY)
    {
        disconnected_count++;
        if (disconnected_count > 10)
        {
            UART_PRINT("did not connect to network after 10 seconds\n\r");
            break;
        }
        sleep(1);
    }
}

static char header[] = "Basic dZdDpXGVz0N0\r\nContent-Type: application/json";
#define HTTP_REQUEST_POST_URI ("/")

network_handle_t network_server_connect(const char * hostname)
{
    int16_t ret;
    network_handle_t handle;

    handle = HTTPClient_create(&ret, 0);
    if (ret != 0)
    {
        UART_PRINT("failed to create HTTP client\n\r");
        return NULL;
    }

    ret = HTTPClient_setHeader(handle, HTTPClient_HFIELD_REQ_AUTHORIZATION, header, 53, HTTPClient_HFIELD_PERSISTENT);
    if (ret != 0)
    {
        UART_PRINT("failed to set header\n\r");
        return NULL;
    }

    ret = HTTPClient_connect(handle, hostname, 0, 0);
    if(ret < 0)
    {
        UART_PRINT("failed to connect to server\n\r");
        return NULL;
    }

    return handle;
}

void network_server_disconnect(const network_handle_t handle)
{
    int16_t ret;

    if (handle == NULL) return;

    ret = HTTPClient_disconnect(handle);
    if(ret < 0)
    {
        UART_PRINT("failed to disconnect from server\n\r");
    }

    HTTPClient_destroy(handle);
}

#define HTTP_RECEIVE_BLOCK_LEN (256)

uint32_t network_send_request(const network_handle_t handle, const char * command, char ** ptr_response)
{
    int16_t ret;
    bool more_data = true;
    uint32_t len = 0;
    *ptr_response = NULL;
    char * curr_loc;

    if (handle == NULL) return 0;

    ret = HTTPClient_sendRequest(handle, HTTP_METHOD_POST, HTTP_REQUEST_POST_URI, command, strlen(command), 0);

    while (more_data == true)
    {
        len += HTTP_RECEIVE_BLOCK_LEN;
        *ptr_response = (char *)realloc(*ptr_response, len);
        if (*ptr_response == NULL)
        {
            UART_PRINT("malloc failed");
            free(*ptr_response);
            return 0;
        }

        curr_loc = *ptr_response + len - HTTP_RECEIVE_BLOCK_LEN;

        ret = HTTPClient_readResponseBody(handle, curr_loc, HTTP_RECEIVE_BLOCK_LEN, &more_data);
        if(ret < 0)
        {
            UART_PRINT("httpTask: response body processing failed: %d", ret);
            free(*ptr_response);
            return 0;
        }
    }

    return len;
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
            status = NETWORK_STATUS_CONNECTED;
            led_connection_on();
            UART_PRINT("Connected\n\r");
            break;

        case SL_WLAN_EVENT_DISCONNECT:
            status = NETWORK_STATUS_DISCONNECTED;
            led_connection_off();
            UART_PRINT("Disconnected\n\r");
            break;

        case SL_WLAN_EVENT_STA_ADDED:
            break;

        case SL_WLAN_EVENT_STA_REMOVED:
            break;

        default:
            break;
    }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Id)
    {
        case SL_NETAPP_EVENT_IPV4_ACQUIRED:
        case SL_NETAPP_EVENT_IPV6_ACQUIRED:
            status = NETWORK_STATUS_READY;
            UART_PRINT("IP Acquired: IP=%d.%d.%d.%d, Gateway=%d.%d.%d.%d\n\r",
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
            break;
        default:
            break;
    }
}

// unused callbacks
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {}
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent) {}
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {}
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent, SlNetAppHttpServerResponse_t * pHttpResponse) {}
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse) {}
void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer) {}
