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
 * @brief  Source file for LVD example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "lvd.h"
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
static void App_LvdPortInit(void);
static void App_LvdInit(void);

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
    ///< LVD 端口初始化
    App_LvdPortInit();
    
    ///< LVD 初始化
    App_LvdInit();
    
    ///< LVD 模块使能
    LVD_Enable(LVD);

    /* 
    ** LVD使能后并非立马可以正常工作，它需要一个建立时间，并且LVD使能之前LVD_OUT
    ** 输出的是默认低电平，当LVD使能时LVD_INPUT电平低于阈值，则使能后会产生一个上
    ** 升沿，由于受建立时间、响应时间、滤波时间的影响，这个上升沿很有可能是执行
    ** LVD_Enable(LVD)后的的几十微妙甚至是几十毫秒后才会到来。
    */
    while (1)
    {
        ;
    }
}


///< LVD 中断服务函数
void Lvd_IRQHandler(void)
{
    ///< 判断中断标志位是否已置位
    if (TRUE == LVD_IsActiveFlag_IT(LVD))
    {
        LVD_ClearFlag_IT(LVD);  ///< 清除中断标志位

        ///< 指示灯闪烁一次，提示用户已发生一次中断
        STK_LED_ON();
        delay1ms(1000);
        STK_LED_OFF();
        delay1ms(1000);
    }
}

///< LVD相关端口初始化
static void App_LvdPortInit(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< GPIOA 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    
    ///< LED端口 初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP; ///< 推挽输出
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    STK_LED_OFF();         
    
    ///< LVD输出端口 初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP; ///< 推挽输出
    stcGpioInit.u32Pin  = GPIO_PIN_02;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOA_Init(&stcGpioInit);
    GPIO_PA02_AF_LVD_OUT_SET();  ///< PA02作为LVD的输出
}

static void App_LvdInit(void)
{
    stc_lvd_init_t stcLvdCfg = {0};

    /* 第一步：LVD 初始化配置 */
    stcLvdCfg.u32TriggerAction = LVD_TRIGGER_ACTION_INT;    ///< 配置触发产生中断
    stcLvdCfg.u32TriggerMode   = LVD_TRIGGER_MODE_HIGHLEVEL;///< 配置高电平触发
    stcLvdCfg.u32InputSource   = LVD_INPUT_AVCC;            ///< 配置LVD输入源为AVCC
    stcLvdCfg.u32ThresholdVolt = LVD_THRESHOLD_VOLT2p4V;    ///< 配置LVD阈值电压为2.4v
    stcLvdCfg.u32Filter        = LVD_FILTER_1023CYCLE;      ///< 配置滤波，滤波宽度小于1023个时钟周期
    LVD_Init(LVD, &stcLvdCfg);
    
    /* 第二步： 中断开启 */
    LVD_ClearFlag_IT(LVD);                              ///< 清除中断标志位
    LVD_EnableIT(LVD);                                  ///< 使能中断
    EnableNvic(LVD_IRQn, IrqLevel3, TRUE);                  ///< 使能中断向量NVIC  
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


