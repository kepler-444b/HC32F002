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

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
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
    ///< 配置LED端口
    App_PortCfg();

    ///< 内核函数，SysTick配置，定时1s
    SysTick_Config(SystemCoreClock);

    while (1)
    {
        ;
    }
}

//systick中断函数
void SysTick_IRQHandler(void)
{
    static boolean_t bFlag;

    if(TRUE == bFlag)
    {
        bFlag = FALSE;
        STK_LED_ON();
    }
    else
    {
        bFlag = TRUE;
        STK_LED_OFF();
    }
}

//配置LED端口
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< GPIOB 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);

    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



