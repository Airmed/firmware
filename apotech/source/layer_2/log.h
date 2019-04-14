
#ifndef __LOG_H__
#define __LOG_H__

typedef enum
{
    LOG_TYPE_DISPENSE_ERROR
} log_type_t;

void log_new(log_type_t type);
void log_send_new();

#endif
