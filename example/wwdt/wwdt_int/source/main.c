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
 * @brief  Source file for WWDT example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/

#include "wwdt.h"
#include "lpm.h"
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
stc_wwdt_init_t gstcWwdtInit = {0};
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_GpioInit(void);
static void App_WdtInit(void);

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
    ///< LED 端口初始化
    App_GpioInit();

    ///< WDT 初始化
    App_WdtInit();
   
    ///< 启动 WDT
    WWDT_Start();

    while (1)
    {
        ;
    }
}


///< WWDT 预溢出中断服务程序
void Wwdt_IRQHandler(void)
{
    if(WWDT_GetPreOverFlag())
    {
        WWDT_Feed(gstcWwdtInit.u32Counter);
        
        WWDT_ClearPreOverFlag();       ///<清除 IWDT 预溢出中断标记
        STK_LED_ON();
        delay1ms(100);
        STK_LED_OFF();
    }
}

static void App_WdtInit(void)
{
    ///< 开启WWDT外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_WWDT);

    ///< WWDT 初始化
    ///< 溢出时间：T_wwdt= ((PCLK分频数 * (CR0.WCNT - 0x3F))/PCLK频率)
    ///< 本例为：(524288 *8/4000000) ≈ 1s
    gstcWwdtInit.u32PreOverInt = WWDT_PRE_INT_ENABLE;
    gstcWwdtInit.u32Counter    = 0x3F + 8;
    gstcWwdtInit.u32Window     = 0x7F;
    gstcWwdtInit.u32Prescaler  = WWDT_PCLK_DIV_524288;
    WWDT_Init(&gstcWwdtInit);
    
    ///< 开启NVIC中断
    EnableNvic(IWDT_WWDT_IRQn, IrqLevel3, TRUE);
}


static void App_GpioInit(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< GPIOB 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);
    
    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    STK_LED_OFF();

}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


