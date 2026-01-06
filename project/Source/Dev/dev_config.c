#include "dev_config.h"
#include "../Source/bsp/bsp_flash.h"
#include "../Source/Bsp/bsp_usart.h"

static panel_cfg_t my_panel_cfg;

void dev_config_save(uint8_t *data, uint16_t len)
{
    app_flash_write(FLASH_ADDR1, data, len);
    dev_config_load();
}

void dev_config_load(void)
{
    app_flash_read(FLASH_ADDR1, (uint8_t *)&my_panel_cfg, sizeof(my_panel_cfg));
    if (my_panel_cfg.dev_addr == 0xFF) {
        my_panel_cfg.dev_addr = 0x00;
    }
    APP_PRINTF("my_panel_cfg.dev_addr:%02X\n", my_panel_cfg.dev_addr);
}

const panel_cfg_t *dev_get_config(void)
{
    return &my_panel_cfg;
}