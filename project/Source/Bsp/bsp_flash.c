#include "bsp_flash.h"

en_result_t app_flash_read(uint32_t address, uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return ErrorInvalidParameter;

    for (uint16_t i = 0; i < length; i++) {
        data[i] = *((uint8_t *)(address + i));
    }

    return Ok;
}

en_result_t app_flash_write(uint32_t address, uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return ErrorInvalidParameter;

    en_result_t ret;

    ret = FLASH_SectorErase(address);
    if (ret != Ok)
        return ret;

    ret = FLASH_WriteByte(address, data, length);
    if (ret != Ok)
        return ret;

    for (uint16_t i = 0; i < length; i++) {
        if (*((uint8_t *)(address + i)) != data[i])
            return Error;
    }

    return Ok;
}