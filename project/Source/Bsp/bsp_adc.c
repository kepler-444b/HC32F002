#include "bsp_adc.h"
#include "adc.h"
#include "../Source/bsp/bsp_pcb.h"

#define ADC_MAX 1024 // 10位精度

void bsp_adc_init(void)
{
    bsp_pcb_adc_init();
}

const uint32_t bsp_get_adc(void)
{
    ADC_StartSingleConversion(ADC);
    while (!ADC_IsActiveFlag_EOC(ADC));
    ADC_ClearFlag_EOC(ADC);
    return (ADC_GetResult(ADC) * 500 / (ADC_MAX - 1));
}