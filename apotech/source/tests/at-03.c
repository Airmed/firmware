
#include <unistd.h>

extern void init_button_interrupts();
extern void init_database_polling();

void * at_03_thread(void * arg0)
{
    init_button_interrupts();
    init_database_polling();

    while (1) sleep(1);

    return 0;
}
