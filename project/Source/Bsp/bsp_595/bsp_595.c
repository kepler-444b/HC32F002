#include "bsp_595.h"
#include "gpio.h"
#include "../Source/Bsp/bsp_pcb.h"

#define SER_SET    GPIO_PC03_SET()
#define SER_RESET  GPIO_PC03_RESET()

#define SRCK_SET   GPIO_PB05_SET()
#define SRCK_RESET GPIO_PB05_RESET()

#define RCK_SET    GPIO_PA01_SET()
#define RCK_RESET  GPIO_PA01_RESET()

void bsp_595_init(void)
{
    bsp_pcb_595_init();
}

void bsp_595_output(uint8_t white, uint8_t yellow)
{
    uint8_t chip1 = 0;
    uint8_t chip2 = 0;

    chip1 |= (yellow & 0x0F) << 0;
    chip1 |= (white & 0x0F) << 4;

    chip2 |= (white & 0x30) >> 4;
    chip2 |= ((yellow & 0x30) >> 4) << 2;

    uint16_t data = ((uint16_t)chip2 << 8) | chip1;

    for (int8_t i = 15; i >= 0; i--) {
        if (data & (1 << i)) {
            SER_SET;
        } else {
            SER_RESET;
        }
        SRCK_SET;
        delay10us(1);
        SRCK_RESET;
        delay10us(1);
    }
    RCK_SET;
    delay10us(1);
    RCK_RESET;
}