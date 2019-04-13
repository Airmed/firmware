
#include "network.h"

#include "uart_term.h"

#include <ti/drivers/net/wifi/simplelink.h>

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    __asm ("bkpt #0");
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
            UART_PRINT("Connected\n\r");
            break;

        case SL_WLAN_EVENT_DISCONNECT:
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

void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    __asm ("bkpt #3");
}

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    __asm ("bkpt #4");
}

void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent, SlNetAppHttpServerResponse_t * pHttpResponse)
{
    __asm ("bkpt #5");
}

void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse)
{
    __asm ("bkpt #6");
}

void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer)
{
    __asm ("bkpt #7");
}

void network_connect()
{
    int32_t ret;
    SlWlanSecParams_t network_params;

    network_params.Type = NETWORK_SECURITY;
    network_params.Key = (int8_t *)NETWORK_PASSWORD;
    network_params.KeyLen = strlen( (char *)network_params.Key );

    ret = sl_WlanConnect((signed char *)NETWORK_SSID, strlen(NETWORK_SSID), NULL, &network_params, NULL);
    if (ret != 0)
    {
        UART_PRINT("failed to connect");
        while(1);
    }
}
