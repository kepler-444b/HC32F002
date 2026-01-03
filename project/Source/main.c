#include "ddl.h"
#include "../Source/Bsp/bsp_usart.h"
#include "../Source/App/app_timer.h"
#include "../Source/Dev/dev_manager.h"
#include "../Source/App/app_protocol.h"

int main(void)
{
    app_timer_init();
    app_protocol_init();
    dev_jump_dev();
    bsp_usart_init(9600);
    while (1) {
        app_timer_poll();
        bsp_usart_has_data();
    }
}
