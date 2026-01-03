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
 * @brief  Source file for GPIO example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "gpio.h"
#include "lpm.h"
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
static void App_LowPowerModeGpioSet(void);

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
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);
    
    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    
    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT | GPIO_MODE_EXTI;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    stcGpioInit.u32ExtI = GPIO_EXTI_FALLING;
    GPIOB_Init(&stcGpioInit);
    
    ///< NVIC 中断使能
    EnableNvic(PORTB_IRQn, IrqLevel0, TRUE);
    
    ///< 进入低功耗模式——深度休眠(使能唤醒后退出中断自动休眠特性)    
    STK_LED_OFF();
    App_LowPowerModeGpioSet();
    LPM_GotoDeepSleep(TRUE);
    
    while(1)
    {
        ;
    }    
    
}

void PortB_IRQHandler(void)
{
    if(STK_USERKEY_PIN | GPIO_ExtIrqStateGet(GPIOB,STK_USERKEY_PIN))
    {
        GPIO_ExtIrqStateClear(GPIOB,STK_USERKEY_PIN);

        STK_LED_ON();
        delay1ms(2000);
        STK_LED_OFF();
    }
}


static void App_LowPowerModeGpioSet(void)
{
    delay1ms(3000);
    ///< 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PC);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);
    
    ///< 数字
    REG_WRITE(GPIOA->ADS, 0x00U);
    REG_WRITE(GPIOB->ADS, 0x00U);
    REG_WRITE(GPIOC->ADS, 0x00U);
    REG_WRITE(GPIOD->ADS, 0x00U);
    
    REG_SETBITS(GPIOB->OUT, 0x00); 
    
    ///< 输入
    REG_WRITE(GPIOA->DIR, 0xFFU);
    REG_WRITE(GPIOB->DIR, 0xEFU);
    REG_WRITE(GPIOC->DIR, 0xFFU);
    REG_WRITE(GPIOD->DIR, 0xFFU);
    
    REG_WRITE(GPIOA->PU, 0xFF);
    REG_WRITE(GPIOB->PU, 0xEF);
    REG_WRITE(GPIOC->PU, 0xFF);
    REG_WRITE(GPIOD->PU, 0xFF);
    
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_SWDIO);
    
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


