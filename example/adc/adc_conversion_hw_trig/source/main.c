/******************************************************************************
 * Copyright (C) 2021, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************/

/******************************************************************************
 * @file   main.c
 *
 * @brief  Source file for ADC example
 *
 * @author MADS Team
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adc.h"
#include "gpio.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint32_t u32AdcRestult;
/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortInit(void);
void App_AdcInit(void);
void App_AdcSqrCfg(void);

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int main(void)
{
    ///< 端口初始化
    App_PortInit();

    ///< ADC模块 初始化
    App_AdcInit();

    while (1) {
        ///< 等待IO外部中断触发ADC转换
        while (TRUE == ADC_IsActiveFlag_EOC(ADC)) ///< EOS中断标志位是否已置位
        {
            ADC_ClearFlag_EOC(ADC); ///< 清除EOC标志位

            ///< 获取采样值
            u32AdcRestult = ADC_GetResult(ADC); ///< 获取采样值

            GPIO_ExtIrqStateClear(GPIOB, GPIO_PIN_03); ///< 清除中断标志位，以便下次收到下降沿时可以继续触发ADC转换
        }
    }
}

///< 端口初始化
void App_PortInit(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< 开启GPIO外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);

    ///< ADC输入端口(PA01)初始化
    GPIO_PA01_ANALOG_SET(); // AIN0

    ///< GPIO IO ADC触发端口(PB03) 初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT | GPIO_MODE_EXTI;
    stcGpioInit.u32Pin  = GPIO_PIN_03;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    stcGpioInit.u32ExtI = GPIO_EXTI_FALLING; // 下降沿触发ADC转换
    GPIOB_Init(&stcGpioInit);
}

///< ADC模块 初始化
void App_AdcInit(void)
{
    stc_adc_cfg_t stcAdcCfg = {0};

    /* 第一步：开启ADC/BGR 外设时钟 */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_ADC);

    /* 第三步：ADC 初始化配置 */
    stcAdcCfg.u32ClkDiv           = ADC_CLOCK_PCLK_DIV128;    ///< 采样分频-128
    stcAdcCfg.u32SampCycleSel     = ADC_SAMPLINGTIME_12CYCLE; ///< 采样周期数-12
    stcAdcCfg.u32RefVolSel        = ADC_REF_VOLTAGE_AVCC;     ///< 参考电压选择-AVCC
    stcAdcCfg.u32InputSource      = ADC_EXINPUT_PA01;         ///< 输入信号选择-PA01
    stcAdcCfg.u32ExtTriggerSource = ADC_EXTTRIGGER_PB03;      ///< 外部触发信号来自-PB03
    ADC_Init(ADC, &stcAdcCfg);

    /* 第四步：清除中断标志位 */
    ADC_ClearFlag_EOC(ADC);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
