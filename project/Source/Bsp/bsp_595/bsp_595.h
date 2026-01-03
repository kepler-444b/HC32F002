#ifndef _BSP_595_H_
#define _BSP_595_H_
#include <stdint.h>

void bsp_595_init(void);
void bsp_595_output(uint8_t white, uint8_t yellow);
#endif