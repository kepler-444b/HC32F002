#ifndef _APP_BASE_H_
#define _APP_BASE_H_

#include <stdint.h>
#include <stdio.h>
uint16_t app_calculate_average(const uint16_t *buffer, uint16_t count);
uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len);
#endif