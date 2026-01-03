#include "dev_manager.h"
#if defined PANEL
#include "dev_panel.h"
#endif

void dev_jump_dev(void)
{
#if defined PANEL
    dev_panel_init();
#endif
}