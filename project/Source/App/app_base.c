#include "app_base.h"

uint16_t app_calculate_average(const uint16_t *buffer, uint16_t count)
{
    if (buffer == NULL || count == 0) {
        return 0;
    }
    uint32_t sum = 0;
    for (uint16_t i = 0; i < count; i++) {
        sum += buffer[i];
    }
    return (uint16_t)(sum / count);
}

uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len)
{
    uint8_t i, sum = 0;
    for (i = 0; i < len; i++)
        sum = sum + rxbuf[i];
    return (0xff - sum + 1);
}
