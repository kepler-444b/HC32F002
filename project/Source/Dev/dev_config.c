#include "dev_config.h"
#include "../Source/bsp/bsp_flash.h"

static panel_cfg_t my_panel_cfg;

void dev_config_save(uint8_t *data, uint16_t len)
{
    app_flash_write(FLASH_ADDR1, data, len);
}

void dev_config_load(void)
{
    app_flash_read(FLASH_ADDR1, (uint8_t *)&my_panel_cfg, sizeof(my_panel_cfg));
}

const panel_cfg_t *dev_get_config(void)
{
    return &my_panel_cfg;
}