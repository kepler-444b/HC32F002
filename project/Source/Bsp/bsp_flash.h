#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_
#include "flash.h"

#define FLASH_ADDR1 0x4000

en_result_t app_flash_read(uint32_t address, uint8_t *data, uint16_t length);
en_result_t app_flash_write(uint32_t address, uint8_t *data, uint16_t length);

#endif
