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
 * @brief  Source file for IWDT example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/

#include "iwdt.h"
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
    IWDT_Start();

    while (1)
    {
        ///< 溢出前按键按下后执行喂狗程序
        if(STK_USERKEY_READ())
        {
            ;
        }
        else
        {
            IWDT_Feed();
            STK_LED_OFF();
        }
        
    }
}


///< IWDT 中断服务程序
void Iwdt_IRQHandler(void)
{
    if(IWDT_GetOverFlag())
    {
        IWDT_ClearOverFlag();       ///<清除 IWDT 中断标记
        STK_LED_ON();
    }
}

static void App_WdtInit(void)
{
    stc_iwdt_init_t stcIwdtInit = {0};

    ///< 开启WDT外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_IWDT);

    ///< IWDT 初始化
    ///< IWDT 溢出时间 = (计数值(u32ArrCounter)/32800) * 分频系数(u32Prescaler)
    ///< 本例为：(3280/32800)*32 = 3.2s
    stcIwdtInit.u32Action = IWDT_OVER_INT__SLEEPMODE_STOP;
    stcIwdtInit.u32ArrCounter = 3280;
    stcIwdtInit.u32Window     = 0xFFF;
    stcIwdtInit.u32Prescaler  = IWDT_RC10K_DIV_32;
    IWDT_Init(&stcIwdtInit);
    
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
    
    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    GPIOB_Init(&stcGpioInit);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


