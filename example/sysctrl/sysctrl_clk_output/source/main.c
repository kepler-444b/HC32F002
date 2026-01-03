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
 * @brief  Source file for SYSCTRL example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "sysctrl.h"
#include "gpio.h"
#include "flash.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_PortCfg(void);
void App_ClkCfg(void);
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** check Pxx to verify the clock frequency.
 **
 ******************************************************************************/
int main(void)
{
    ///< 端口配置(时钟输出引脚)
    App_PortCfg();

    ///< 系统时钟配置
    App_ClkCfg();

    ///< 时钟输出
    GPIO_PA01_HCLK_OUT();
    GPIO_PC04_TCLK_OUT_PCLK_DIV8();

    while (1)
    {
        ;
    }
}


void App_ClkCfg(void)
{
    stc_sysctrl_sysclk_source_init_t stcSysClkSrc = {0};
    stc_sysctrl_clk_init_t stcSysClk = {0};

    ///< 系统时钟源初始化
    stcSysClkSrc.u32SysClkSourceType = SYSCTRL_SYSCLK_SOURCE_TYPE_RCH;
    stcSysClkSrc.u32RCHState         = SYSCTRL_RCH_TRIM_48MHz |\
                                       SYSCTRL_RCH_DIV12;

    SYSCTRL_SysClkSrcInit(&stcSysClkSrc);

    ///< 系统时钟初始化
    stcSysClk.u32ClockType    = SYSCTRL_CLOCKTYPE_SYSCLK |\
                                SYSCTRL_CLOCKTYPE_HCLK   |\
                                SYSCTRL_CLOCKTYPE_PCLK;
    stcSysClk.u32SysClkSource = SYSCTRL_SYSCLK_SOURCE_RCH;
    stcSysClk.u32HClkDiv      = SYSCTRL_SYSCLK_HCLK_PRS1;
    stcSysClk.u32PClkDiv      = SYSCTRL_SYSCLK_PCLK_PRS2;
    stcSysClk.u32WaitCycle    = SYSCTRL_FLASH_WAIT_CYCLE1;

    SYSCTRL_SysClkInit(&stcSysClk);
}

//端口配置，按键按下，继续运行
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< GPIOA 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA | SYSCTRL_PERICLK_PC);

    ///< 时钟输出端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_01;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOA_Init(&stcGpioInit);
    
    stcGpioInit.u32Pin = GPIO_PIN_04;
    GPIOC_Init(&stcGpioInit);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



