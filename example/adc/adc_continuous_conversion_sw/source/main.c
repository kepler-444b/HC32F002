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
    ///< 时钟初始化,此应用使用默认4MHz时钟，不需要配置
    //App_ClkInit();
    
    ///< 端口初始化
    App_PortInit();
    
    ///< ADC模块 初始化
    App_AdcInit();
    
    ADC_StartContinuousConversion(ADC);  //开启连续转换，每转换一次,产生EOC标志
    
    delay1ms(100);                           //延时100ms
    
    ADC_StopContinuousConversion(ADC);   //停止ADC转换
    
    while(1)
    {   
        ;
    }
}

 ///< ADC 中断服务程序
void Adc_IRQHandler(void)
{        
    /* 判断是否产生EOS标志 */
    if(TRUE == ADC_IsActiveFlag_EOC(ADC))
    {
        ADC_ClearFlag_EOC(ADC);              ///< 清除EOC中断标志位

        u32AdcRestult = ADC_GetResult(ADC);  ///< 获取采样值
    }
}

///< 端口初始化
void App_PortInit(void)
{    
    ///< 开启GPIO外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    
    ///< 模拟端口输入信号初始化
    GPIO_PA01_ANALOG_SET();  
}

///< ADC模块 初始化
void App_AdcInit(void)
{
    stc_adc_cfg_t   stcAdcCfg={0};
    
    /* 第一步：开启ADC/BGR 外设时钟 */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_ADC);
    
    /* 第三步：ADC 初始化配置 */
    stcAdcCfg.u32ClkDiv           = ADC_CLOCK_PCLK_DIV128;     ///< 采样分频-128
    stcAdcCfg.u32SampCycleSel     = ADC_SAMPLINGTIME_12CYCLE;  ///< 采样周期数-12
    stcAdcCfg.u32RefVolSel        = ADC_REF_VOLTAGE_AVCC;      ///< 参考电压选择-AVCC
    stcAdcCfg.u32InputSource      = ADC_EXINPUT_PA01;          ///< 输入信号选择-PA01
    //stcAdcCfg.u32ExtTriggerSource = ADC_EXTTRIGGER_PB03;       ///< 外部触发信号来自-PB03
    ADC_Init(ADC, &stcAdcCfg);
    
    /* 第四步：清除中断标志位 */
    ADC_ClearFlag_EOC(ADC);
    
    /* 第四步：配置中断 */
    ADC_EnableIT_EOC(ADC);      ///< 使能EOC中断
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);  ///< 使能内核NVIC
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


