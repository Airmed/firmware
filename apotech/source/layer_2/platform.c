/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>
//#include <ti/display/Display.h>
#include <ti/drivers/SPI.h>

#include "Board.h"
#include "pthread.h"
#include "semaphore.h"

#define APPLICATION_NAME                      "Database Integration"
#define DEVICE_ERROR                          ("Device error, please refer \"DEVICE ERRORS CODES\" section in errors.h")
#define WLAN_ERROR                            ("WLAN error, please refer \"WLAN ERRORS CODES\" section in errors.h")
#define SL_STOP_TIMEOUT                       (200)
#define SPAWN_TASK_PRIORITY                   (9)
#define SPAWN_STACK_SIZE                      (4096)
#define TASK_STACK_SIZE                       (2048)
#define SLNET_IF_WIFI_PRIO                    (5)
#define SLNET_IF_WIFI_NAME                    "CC32xx"
/* AP SSID */
#define SSID_NAME                             "IT_Test"

/* Security type could be SL_WLAN_SEC_TYPE_WPA_WPA2 */      
#define SECURITY_TYPE                         SL_WLAN_SEC_TYPE_WPA_WPA2 /* SL_WLAN_SEC_TYPE_OPEN */ 

/* Password of the secured AP */
#define SECURITY_KEY                          "airmed_test"       

#define builtBuffPush           "{\"query\":\"insert into test(one, two, three, four) values (101, 102, 103, 104);\"}" /* Push data to database */
#define builtBuffPull           "{\"query\":\"select * from test;\"}" /* Get data from database */

pthread_t httpThread = (pthread_t)NULL;
pthread_t spawn_thread = (pthread_t)NULL;

int32_t mode;
//Display_Handle display;

extern void* httpTaskPost(void* pvParameters);
extern void* httpTaskPull(void* pvParameters);
extern const char *WlanStatus[];

/*
 *  ======== printError ========
 */
void printError(char *errString,
                int code)
{
    //Display_printf(display, 0, 0, "Error! code = %d, Description = %s\n", code,
                  // errString);
    while(1)
    {
        ;
    }
}

/*!
    \brief          SimpleLinkNetAppEventHandler

    This handler gets called whenever a Netapp event is reported
    by the host driver / NWP. Here user can implement he's own logic
    for any of these events. This handler is used by 'network_terminal'
    application to show case the following scenarios:

    1. Handling IPv4 / IPv6 IP address acquisition.
    2. Handling IPv4 / IPv6 IP address Dropping.

    \param          pNetAppEvent     -   pointer to Netapp event data.

    \return         void

    \note           For more information, please refer to: user.h in the porting
                    folder of the host driver and the  CC31xx/CC32xx
                    NWP programmer's
                    guide (SWRU455) section 5.7

 */
void SimpleLinkNetAppEventHandlerUnused(SlNetAppEvent_t *pNetAppEvent)
{
    int32_t             status = 0;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;

    if(pNetAppEvent == NULL)
    {
        return;
    }

    switch(pNetAppEvent->Id)
    {
    case SL_NETAPP_EVENT_IPV4_ACQUIRED:
    case SL_NETAPP_EVENT_IPV6_ACQUIRED:
        /* Initialize SlNetSock layer with CC3x20 interface                   */
        SlNetIf_init(0);
        SlNetIf_add(SLNETIF_ID_1, SLNET_IF_WIFI_NAME, 
                   (const SlNetIf_Config_t *)&SlNetIfConfigWifi,
                    SLNET_IF_WIFI_PRIO);

        SlNetSock_init(0);
        SlNetUtil_init(0);
        if(mode != ROLE_AP)
        {
            /*Display_printf(display, 0, 0,"[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                        "Gateway=%d.%d.%d.%d\n\r",
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
                        */

            pthread_attr_init(&pAttrs);
            priParam.sched_priority = 1;
            status = pthread_attr_setschedparam(&pAttrs, &priParam);
            status |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE);

            status = pthread_create(&httpThread, &pAttrs, httpTaskPost, NULL);
            pthread_join(&httpThread, NULL);
            status = pthread_create(&httpThread, &pAttrs, httpTaskPull, NULL);


            if(status)
            {
                printError("Task create failed", status);
            }
        }
        break;
    default:
        break;
    }
}


void Connect(void)
{
    SlWlanSecParams_t secParams = {0};
    int16_t ret = 0;
    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;
    //Display_printf(display, 0, 0, "Connecting to : %s.\r\n",SSID_NAME);
    ret = sl_WlanConnect((signed char*)SSID_NAME, strlen(
                             SSID_NAME), 0, &secParams, 0);
    if(ret)
    {
        printError("Connection failed", ret);
    }
}

/*!
    \brief          Display application banner

    This routine shows application startup display on UART.

    \param          appName    -   points to a string representing application name.


*/
static void DisplayBanner(char * AppName)
{
    /*
    Display_printf(display, 0, 0, "\n\n\n\r");
    Display_printf(display, 0, 0,
                   "\t\t *************************"
                   "************************\n\r");
    Display_printf(display, 0, 0, "\t\t            %s Application       \n\r",
                   AppName);
    Display_printf(display, 0, 0,
                   "\t\t **************************"
                   "***********************\n\r");
    Display_printf(display, 0, 0, "\n\n\n\r");
    */
}


void mainThread(void *pvParameters)
{
    int32_t status = 0;
    pthread_attr_t pAttrs_spawn;
    struct sched_param priParam;
	
    SPI_init();
    /*
    Display_init();
    display = Display_open(Display_Type_UART, NULL);
    if(display == NULL)
    {
        while(1)
        {
            ;
        }
    }
*/

    /* Print Application name */
    DisplayBanner(APPLICATION_NAME);

    /* Start the SimpleLink Host */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    status = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    status |= pthread_attr_setstacksize(&pAttrs_spawn, SPAWN_STACK_SIZE);

    status = pthread_create(&spawn_thread, &pAttrs_spawn, sl_Task, NULL);
    if(status)
    {
        printError("Task create failed", status);
    }

    /* Turn NWP on - initialize the device*/
    mode = sl_Start(0, 0, 0);
    if (mode < 0)
    {
        //Display_printf(display, 0, 0,"\n\r[line:%d, error code:%d] %s\n\r", __LINE__, mode, DEVICE_ERROR);
    }

    if(mode != ROLE_STA)
    {
        /* Set NWP role as STA */
        mode = sl_WlanSetMode(ROLE_STA);
        if (mode < 0)
        {
            //Display_printf(display, 0, 0,"\n\r[line:%d, error code:%d] %s\n\r", __LINE__, mode, WLAN_ERROR);
        }

        /* For changes to take affect, we restart the NWP */
        status = sl_Stop(SL_STOP_TIMEOUT);
        if (status < 0)
        {
            //Display_printf(display, 0, 0,"\n\r[line:%d, error code:%d] %s\n\r", __LINE__, status, DEVICE_ERROR);
        }

        mode = sl_Start(0, 0, 0);
        if (mode < 0)
        {
            //Display_printf(display, 0, 0,"\n\r[line:%d, error code:%d] %s\n\r", __LINE__, mode, DEVICE_ERROR);
        }
    }

    if(mode != ROLE_STA)
    {
        printError("Failed to configure device to it's default state", mode);
    }
    Connect();
}
