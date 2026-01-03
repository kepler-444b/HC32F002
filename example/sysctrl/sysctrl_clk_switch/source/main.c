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

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortCfg(void);

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
    stc_sysctrl_sysclk_source_init_t stcSysClkSrc = {0};
    stc_sysctrl_clk_init_t stcSysClk = {0};
    
    //端口配置(外部晶体引脚/时钟输出引脚)
    App_PortCfg();
    
    ///< 系统时钟源初始化
    stcSysClkSrc.u32SysClkSourceType = SYSCTRL_SYSCLK_SOURCE_TYPE_RCH |\
                                       SYSCTRL_SYSCLK_SOURCE_TYPE_RCL;

    stcSysClkSrc.u32RCHState         = SYSCTRL_RCH_TRIM_48MHz |\
                                       SYSCTRL_RCH_DIV12;

    stcSysClkSrc.u32RCLState         = SYSCTRL_RCL_TRIM_32p8KHz |\
                                       SYSCTRL_RCL_WAITCYCLE256;
    
    SYSCTRL_SysClkSrcInit(&stcSysClkSrc);
    
    ///< 时钟输出
    GPIO_PA01_HCLK_OUT();
    delay1ms(2000);
    
    ///< 系统时钟初始化
    stcSysClk.u32ClockType    = SYSCTRL_CLOCKTYPE_SYSCLK |\
                                SYSCTRL_CLOCKTYPE_HCLK   |\
                                SYSCTRL_CLOCKTYPE_PCLK;
    stcSysClk.u32SysClkSource = SYSCTRL_SYSCLK_SOURCE_RCL;
    stcSysClk.u32HClkDiv      = SYSCTRL_SYSCLK_HCLK_PRS1;
    stcSysClk.u32PClkDiv      = SYSCTRL_SYSCLK_PCLK_PRS2;
    stcSysClk.u32WaitCycle    = SYSCTRL_FLASH_WAIT_CYCLE1;
    
    SYSCTRL_SysClkInit(&stcSysClk);
    
    
    while (1)
    {
        ;
    }
}


//端口配置，HCLK输出
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< GPIOA 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA);
    
    ///< 时钟输出端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_01;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOA_Init(&stcGpioInit);
    
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



