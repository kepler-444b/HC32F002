#include "bsp_pcb.h"
#include "gpio.h"
#include "adc.h"
#include "../Source/Dev/dev_manager.h"
void bsp_pcb_panel_init(void)
{

    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);

    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);

    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT | GPIO_MODE_EXTI;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    stcGpioInit.u32ExtI = GPIO_EXTI_RISING_FALLING;
    GPIOB_Init(&stcGpioInit);
}

void bsp_pcb_RD_init(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PC);
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_04;
    GPIOC_Init(&stcGpioInit);
}

void bsp_pcb_usart_init(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PD);

    GPIO_PD05_SET();
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP; // TX
    stcGpioInit.u32Pin  = GPIO_PIN_05;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOD_Init(&stcGpioInit);
    stcGpioInit.u32Mode = GPIO_MODE_INPUT; // RX
    stcGpioInit.u32Pin  = GPIO_PIN_06;
    GPIOD_Init(&stcGpioInit);
    GPIO_PD05_AF_LPUART1_TXD_SET();
    GPIO_PD06_AF_LPUART1_RXD_SET();
}

void bsp_pcb_adc_init(void)
{
    stc_adc_cfg_t stcGpioInit = {0};
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_ADC);
    stcGpioInit.u32ClkDiv       = ADC_CLOCK_PCLK_DIV128;
    stcGpioInit.u32SampCycleSel = ADC_SAMPLINGTIME_6CYCLE;
    stcGpioInit.u32RefVolSel    = ADC_REF_VOLTAGE_AVCC;
    stcGpioInit.u32InputSource  = ADC_EXINPUT_PA02;
    ADC_Init(ADC, &stcGpioInit);
    ADC_ClearFlag_EOC(ADC);
}

void bsp_pcb_595_init(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PC);
    // RCK
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_01;
    GPIOA_Init(&stcGpioInit);
    // SRCK
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_05;
    GPIOB_Init(&stcGpioInit);
    // SER
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_03;
    GPIOC_Init(&stcGpioInit);
}