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
 * @brief  Source file for CTRIM example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "ctrim.h"
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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
 
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void App_TrimCalInit(void);
static void App_GpioCfg(void);

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return int32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int main(void)
{
    ///< 时钟初始化,使用系统默认4MHz，不需要配置
    //App_ClkInit();
    
    ///< 端口初始化
    App_GpioCfg();
    
    ///< 等待 USER按键 按下
    while (0 != STK_USERKEY_READ());  
    
    ///< Trim 校准功能初始化
    App_TrimCalInit();    
    
    ///< 使能CTRIM模块，开启校验
    CTRIM_Enable(CTRIM);
    
    while(1)
    {
        ;
    }    
}


/*******************************************************************************
 * TRIM 中断服务程序
 ******************************************************************************/   
void Ctrim_IRQHandler(void)
{
   if (TRUE == CTRIM_IsActiveFlag(CTRIM, CTRIM_FLAG_UD))
   {
       CTRIM_ClearFlag(CTRIM, CTRIM_CLEAR_FLAG_UD);  ///< 清除标志位
       
       ///< LED灯闪烁一次
       STK_LED_ON();     //开启LED
       delay1ms(200);
       STK_LED_OFF();    //关闭LED
   }
   
}

//端口相关配置
void App_GpioCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    
    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    STK_LED_OFF();     //关闭LED
    
    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);       
}


static void App_TrimCalInit(void)
{
    stc_ctrim_timer_init_t stcCfg = {0};

    /* 第一步：开启CTRIM 外设时钟 */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_CTRIM);
    
    /* 第二步：复位CTRIM模块 */
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_CTRIM);
     
    /* 第三步：TRIM 初始化配置 */
    stcCfg.u32Clock      = CTRIM_ACCURATE_CLOCK_PCLK;
    stcCfg.u32ClockDiv   = CTRIM_REF_CLOCK_DIV1024;       ///< 分频1024
    //stcCfg.u32EtrFilter  = CTRIM_ETR_CLOCK_FILTER_NONE; ///< 外部时钟未使用，可以不配置
    stcCfg.u32ReloadVal         = 3906-1;                 ///< 定时约1s，4000000/1024 = 3096
    CTRIM_TimerInit(CTRIM, &stcCfg);
    
    /* 第四步：清除所有标志位 */
    CTRIM_ClearFlag_ALL(CTRIM);
    
    /* 第五步：使能中断 */
    CTRIM_EnableIT(CTRIM, CTRIM_IT_UD);               ///< 计数器下溢中断 使能
    EnableNvic(CTRIM_IRQn, IrqLevel3, TRUE);              ///< 使能并配置TRIM 系统中断
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


