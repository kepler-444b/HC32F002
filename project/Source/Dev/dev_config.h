#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_
#include "../Source/Dev/dev_manager.h"
#include <stdint.h>

#if defined PANEL_A13

#define PANEL_ADDR_MAX 32

#define PANEL_VOL_RANGE_DEF          \
    [0] = {.vol_range = {0, 5}},     \
    [1] = {.vol_range = {50, 55}},   \
    [2] = {.vol_range = {138, 143}}, \
    [3] = {.vol_range = {228, 233}}, \
    [4] = {.vol_range = {307, 332}}, \
    [5] = {.vol_range = {366, 371}}

#endif

typedef struct
{
    uint8_t dev_addr;
} panel_cfg_t;

void dev_config_save(uint8_t *addr, uint16_t len);
void dev_config_load(void);
const panel_cfg_t *dev_get_config(void);

#endif