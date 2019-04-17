
#include "database.h"

#include "data_types.h"
#include "network.h"
#include "uart_term.h"
#include "file.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/utils/json/json.h>

#define DATABASE_HOSTNAME ("https://seniordesigndb.herokuapp.com")
#define DATABASE_MAX_COMMAND_LEN (128)
#define DATABASE_TIMESTAMP_LEN (27)
#define DATABASE_MAX_TYPE_STR_LEN (32)

static const char str_query_medications[] = "{\"query\":\"SELECT * FROM medications;\"}";
static const char str_update_medication_qty[] = "{\"query\":\"UPDATE medications SET qty=%d WHERE id=%d;\"}";
static const char str_query_schedule[] = "{\"query\":\"SELECT * FROM schedule;\"}";
static const char str_add_log[] = "{\"query\":\"INSERT INTO history (cur_time, error) VALUES (%lu, %s);\"}";
static const char str_add_notification[] = "{\"query\":\"INSERT INTO notifications (cur_time, error) VALUES (%s, %s);\"}";

Json_Handle templateHandle;
Json_Handle jsonObjHandle;
char* templateBuff;
char* jsonBuffer;
int16_t templateSize;
uint16_t objSize;

typedef struct
{
    uint32_t temp;
} database_schedule_t;


typedef enum
{
    DATABASE_LOG_TYPE_DISPENSE_ERROR,
    DATABASE_LOG_TYPE_DISPENSE_SUCCESS,
    DATABASE_LOG_TYPE_PILLS_TAKEN
} database_log_type_t;

typedef enum
{
    INT32 = 0,
    STRING_RAW = 1,
    BOOLEAN = 2
}json_value_t;

int rows; /* Calculate number of rows in database */

static uint8_t database_read_medications(database_medication_t ** ptr_medication_arr)
{
    uint32_t num_medications = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_medications, &response);
    network_server_disconnect(handle);

    UART_PRINT("%s\n\r", response);
    parse_medication(response, ptr_medication_arr);
    //*ptr_medication_arr =

    free(response);

    return num_medications;
}

static uint8_t database_read_schedule(database_schedule_t ** ptr_schedule_arr)
{
    uint32_t num_schedule = 0;
    char * response;

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    uint32_t response_len = network_send_request(handle, str_query_schedule, &response);
    network_server_disconnect(handle);

    // replace with parser
    UART_PRINT("%s\n\r", response);
    parse_schedule(response, ptr_schedule_arr);
    //*ptr_schedule_arr =

    free(response);

    return num_schedule;
}

configuration_t database_read_configuration()
{
    database_medication_t * medication_arr;
    database_schedule_t * schedue_arr;

    uint8_t num_medications = database_read_medications(&medication_arr);
    uint8_t num_schedule = database_read_schedule(&schedue_arr);

    configuration_t configuration;
    memset(&configuration, 0, sizeof(configuration_t));

    // TODO translate

//    free(medication_arr);
//    free(schedue_arr);

    return configuration;
}

void database_write_medication_qty(uint8_t med_id, uint8_t qty)
{
    char command[DATABASE_MAX_COMMAND_LEN];
    char * response;

    sprintf(command, str_update_medication_qty, qty, med_id);

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    network_send_request(handle, command, &response);
    network_server_disconnect(handle);

    free(response);
}

void database_write_log(log_t log, bool notify)
{
    char command[DATABASE_MAX_COMMAND_LEN];
    char type_str[DATABASE_MAX_TYPE_STR_LEN];
    char timestamp_str[DATABASE_TIMESTAMP_LEN];
    char * response;

    // TODO time handling
//    sprintf(timestamp_str, "%04d-%02d-%02d %02d:%02d:%02d.000000", log.year, log.month, log.day, log.hour, log.minute, log.second);

    switch (log.type)
    {
        case DATABASE_LOG_TYPE_DISPENSE_ERROR:
            strcpy(type_str, "Dispense Error");
            break;
        case DATABASE_LOG_TYPE_DISPENSE_SUCCESS:
            strcpy(type_str, "Dispense Success");
            break;
        case DATABASE_LOG_TYPE_PILLS_TAKEN:
            strcpy(type_str, "Pills Taken");
            break;
    }

    sprintf(command, str_add_log, log.time, type_str);

    network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
    network_send_request(handle, command, &response);
    network_server_disconnect(handle);

    free(response);

    if (notify == true)
    {
        sprintf(command, str_add_notification, timestamp_str, type_str);

        network_handle_t handle = network_server_connect(DATABASE_HOSTNAME);
        network_send_request(handle, command, &response);
        network_server_disconnect(handle);

        free(response);
    }
}


//Zero out struct before parsing
void parse_medication(char * response, database_medication_t **medicationPtr){
    *medicationPtr = malloc(4*(sizeof(database_medication_t)));

    char *template_medication = "{"
                            "\"success\":boolean,"
                            "\"data\":[{\"id\":int32,\"name\":string,\"qty\":int32}]}";
    int16_t retVal;
    uint16_t objSize = 1024;
    char keyName[100];
    char keyQty[100];
    uint16_t valueSize = 100;
    char getBuffer[100];
    int32_t numValue;


    retVal = Json_createTemplate(&templateHandle, template_medication, strlen(template_medication));
    if(retVal < 0){
        UART_PRINT("Error Creating Template");
        while(1)
                {
                    ;
                }
    }
    retVal = Json_createObject(&jsonObjHandle,templateHandle,objSize);
    if(retVal < 0){
            UART_PRINT("Error Creating Object");
            while(1)
                    {
                        ;
                    }
        }

   UART_PRINT("Response: %s", response);
    retVal = Json_parse(jsonObjHandle, response, strlen(response));

    if(retVal < 0){
            UART_PRINT("Error Parsing Data");
            while(1)
                    {
                        ;
                    }
        }


        /* Parse Name */
    rows = 0;
    do {
            memset(keyName,'\0',100);
            memset(getBuffer,'\0',100);
            sprintf(keyName, "\"data\".[%d].\"name\"", rows);
            valueSize=100;
            retVal = Json_getValue(jsonObjHandle,keyName,getBuffer,&valueSize);
            //(*medicationPtr)[i].name = getBuffer;
            UART_PRINT("\nName: %s \n\r",getBuffer);
            rows++;
       } while(strcmp (getBuffer,"") != 0);
       /* Parse Quantity */

       /* Parse Qty */
       int i = 0;
       int noData = 0;
       int16_t Val = Json_getArrayMembersCount(jsonObjHandle, "\"data\".[0].\"qty\"");
       int16_t checkVal = Val;
           while(noData == 0){
                 sprintf(keyQty, "\"data\".[%d].\"qty\"", i);
                 valueSize=100;
                 retVal = Json_getValue(jsonObjHandle,keyQty,&numValue,&valueSize);
                 UART_PRINT("Qty: %d \n\r",numValue);
                 i++;
                 checkVal = Val;
                 sprintf(keyQty, "\"data\".[%d].\"qty\"", i);
                 int16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyQty);
                 if(Val != checkVal) /* Check if data still available */
                     noData = 1;
                 //(*medicationPtr)[i].count = numValue;
          }

    retVal = Json_destroyTemplate(templateHandle);
    retVal = Json_destroyObject(jsonObjHandle);

}

void parse_schedule(char * response, dispense_slot_t **medicationPtr){

    char *template_schedule = "{"
                         "\"success\":boolean,"
                         "\"data\":[{\"id\":int32,\"med_id\":int32,\"hour\":int32,\"minute\":int32, \"num_disp\":int32}]}";

    int16_t retVal;
    uint16_t objSize = 1024;
    char keyMedId[100];
    char keyHour[100];
    char keyMinute[100];
    uint16_t valueSize = 100;
    char getBuffer[100];
    int32_t numValue;

    retVal = Json_createTemplate(&templateHandle, template_schedule, strlen(template_schedule));
    if(retVal < 0){
        UART_PRINT("Error Creating Template");
        while(1)
                {
                    ;
                }
    }
    retVal = Json_createObject(&jsonObjHandle,templateHandle,objSize);
    if(retVal < 0){
            UART_PRINT("Error Creating Object");
            while(1)
                    {
                        ;
                    }
        }

    retVal = Json_parse(jsonObjHandle, response, strlen(response));

    if(retVal < 0){
            UART_PRINT("Error Parsing Data");
            while(1)
                    {
                        ;
                    }
        }

    /* Parse med_id */
    int i = 0;
    int noData = 0;
    int16_t Val = Json_getArrayMembersCount(jsonObjHandle, "\"data\".[0].\"med_id\"");
    int16_t checkVal = Val;
        while(noData == 0){
              sprintf(keyMedId, "\"data\".[%d].\"med_id\"", i);
              valueSize=100;
              retVal = Json_getValue(jsonObjHandle,keyMedId,&numValue,&valueSize);
              UART_PRINT("Med_Id: %d \n\r",numValue);
              i++;
              checkVal = Val;
              sprintf(keyMedId, "\"data\".[%d].\"med_id\"", i);
              int16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyMedId);
              if(Val != checkVal) /* Check if data still available */
                  noData = 1;
              //(*medicationPtr)[i].count = numValue;
       }

        /* Parse Hour*/
        noData = 0;
        i = 0;
        Val = Json_getArrayMembersCount(jsonObjHandle, "\"data\".[0].\"hour\"");
        checkVal = Val;
        while(noData == 0){
              sprintf(keyHour, "\"data\".[%d].\"hour\"", i);
              valueSize=100;
              retVal = Json_getValue(jsonObjHandle,keyHour,&numValue,&valueSize);
              UART_PRINT("Hour: %d \n\r",numValue);
              i++;
              checkVal = Val;
              sprintf(keyHour, "\"data\".[%d].\"hour\"", i);
              int16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyHour);
              if(Val != checkVal) /* Check if data still available */
                  noData = 1;
              //(*medicationPtr)[i].count = numValue;
       }

        /* Parse Minute */
        noData = 0;
        i = 0;
        Val = Json_getArrayMembersCount(jsonObjHandle, "\"data\".[0].\"minute\"");
        checkVal = Val;
        while(noData == 0){
              sprintf(keyMinute, "\"data\".[%d].\"minute\"", i);
              valueSize=100;
              retVal = Json_getValue(jsonObjHandle,keyMinute,&numValue,&valueSize);
              UART_PRINT("Minute: %d \n\r",numValue);
              i++;
              checkVal = Val;
              sprintf(keyMinute, "\"data\".[%d].\"minute\"", i);
              int16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyMinute);
              if(Val != checkVal) /* Check if data still available */
                  noData = 1;
              //(*medicationPtr)[i].count = numValue;
       }
        //getValue("\"data\".[%d].\"hour\"");

    retVal = Json_destroyTemplate(templateHandle);
    retVal = Json_destroyObject(jsonObjHandle);

}

#if 0 /* Wrote a function to parse but you need the integers anyways so whatever */
void getValue(char* key){
    int noData = 0;
    int i = 0;
    char *keyCopy[100];
    uint16_t valueSize = 100;
    char getBuffer[100];
    int32_t numValue;
    int16_t retVal;


    sprintf(keyCopy, key, 0);
    uint16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyCopy);
    uint16_t checkVal = Val;
    while(noData == 0){
          sprintf(keyCopy, key, i);
          valueSize=100;
          retVal = Json_getValue(jsonObjHandle,keyCopy,&numValue,&valueSize);
          i++;
          checkVal = Val;
          sprintf(keyCopy, key, i);
          int16_t Val = Json_getArrayMembersCount(jsonObjHandle, keyCopy);
          if(Val != checkVal) /* Check if data still available */
              noData = 1;
          //(*medicationPtr)[i].count = numValue;
   }

}
#endif
