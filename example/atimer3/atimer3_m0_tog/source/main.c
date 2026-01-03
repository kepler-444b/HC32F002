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
 * @brief  Source file for ATIM3 example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "atim3.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_Clock_Cfg(void);
void App_Atimer3_Port_Cfg(void);
void App_Atimer3_Cfg(uint16_t u16Period);

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

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
    App_Clock_Cfg();  ////时钟初始化
    //注意！！！！如果时钟超过24M,需要设置flash wait为2个周期！！！！！！！！！
  
    App_Atimer3_Port_Cfg(); //ATIM3 端口配置

    App_Atimer3_Cfg(25000); //ATIM3 配置; 16分频,周期25000-->25000*(1/4M) * 16 = 100000us = 100ms
    
    ATIM3_Mode0_Run();      //ATIM3 运行。
    
    //可添加其他其他操作，待准备好后使能TOG输出
    
    ATIM3_Mode0_Enable_TOG(TRUE);  //翻转输出使能
    
    while (1);
}
/*******************************************************************************
 * ATIM3中断服务函数
 ******************************************************************************/
void ATim3_IRQHandler(void)
{
    //ATIM3 模式0 计时溢出中断
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_UI))
    {
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI);  //ATIM3 模式0 中断标志清除
    }
}

//时钟初始化
//注意！！！！如果时钟超过24M,需要设置flash wait为2个周期！！！！！！！！！
void App_Clock_Cfg(void)
{
    stc_sysctrl_sysclk_source_init_t   stcSysClkSrc = {0};
    stc_sysctrl_clk_init_t             stcSysClk = {0};

    ///< 系统时钟源初始化
    stcSysClkSrc.u32SysClkSourceType = SYSCTRL_SYSCLK_SOURCE_TYPE_RCH;   //时钟源初始化需要操作RCH
        
    stcSysClkSrc.u32RCHState         = SYSCTRL_RCH_TRIM_48MHz |\
                                       SYSCTRL_RCH_DIV12;            //RCH 设置为12分频  -->4M,
    SYSCTRL_SysClkSrcInit(&stcSysClkSrc);   //时钟源初始化
    
    ///< 系统时钟初始化
    stcSysClk.u32ClockType    = SYSCTRL_CLOCKTYPE_SYSCLK |\
                                SYSCTRL_CLOCKTYPE_HCLK   |\
                                SYSCTRL_CLOCKTYPE_PCLK;    //时钟初始化需要操作PLL，HCLK和PCLK
    stcSysClk.u32SysClkSource = SYSCTRL_SYSCLK_SOURCE_RCH; //RCH为系统时钟
    stcSysClk.u32HClkDiv      = SYSCTRL_SYSCLK_HCLK_PRS1;  //HCLK不分频
    stcSysClk.u32PClkDiv      = SYSCTRL_SYSCLK_PCLK_PRS1;  //PCLK不分频
    stcSysClk.u32WaitCycle    = SYSCTRL_FLASH_WAIT_CYCLE1; //设置flash 读等待为1个周期
    
    SYSCTRL_SysClkInit(&stcSysClk);     //时钟初始化
}

//ATIM3 CHx 端口配置
void App_Atimer3_Port_Cfg(void)
{
    stc_gpio_init_t stcCHAGpioInit = {0};
    stc_gpio_init_t stcCHBGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);  
    
    ///< PD05(AIMT3_CH0A)端口初始化
    stcCHAGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHAGpioInit.u32Pin  = GPIO_PIN_05;
    stcCHAGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOD_Init(&stcCHAGpioInit);
    
    ///< PB00(AIMT3_CH0B)端口初始化
    stcCHBGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHBGpioInit.u32Pin  = GPIO_PIN_00;
    stcCHBGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcCHBGpioInit);
    
    GPIO_PD05_AF_ATIM3_CH0A_SET();
    GPIO_PB00_AF_ATIM3_CH0B_SET();
}

//ATIM3 配置
void App_Atimer3_Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_atim3_mode0_cfg_t     stcAtim3BaseCfg = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3); //ATIM3 外设时钟使能
    
    stcAtim3BaseCfg.u32WorkMode     = ATIM3_M0_M0CR_WORK_MODE_TIMER;  //定时器模式
    stcAtim3BaseCfg.u32CountClkSel  = ATIM3_M0_M0CR_CT_PCLK;          //定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS          = ATIM3_M0_M0CR_ATIM3CLK_PRS16;   //PCLK/16
    stcAtim3BaseCfg.u32CntMode      = ATIM3_M0_M0CR_MD_16BIT_ARR;     //自动重载16位计数器/定时器
    stcAtim3BaseCfg.u32EnTog        = ATIM3_M0_M0CR_TOG_OFF;          //TOG使能
    stcAtim3BaseCfg.u32EnGate       = ATIM3_M0_M0CR_GATE_OFF;         //门控功能关闭
    stcAtim3BaseCfg.u32GatePolar    = ATIM3_M0_M0CR_GATE_POLAR_HIGH;  //
    
    ATIM3_Mode0_Init(&stcAtim3BaseCfg);         //ATIM3 的模式0功能初始化
        
    u16ArrValue = 0x10000 - u16Period;
    ATIM3_Mode0_ARRSet(u16ArrValue);            //设置重载值
    
    u16CntValue = 0x10000 - u16Period;
    ATIM3_Mode0_Cnt16Set(u16CntValue);          //设置计数初值
    
    //ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI);     //清中断标志
    //ATIM3_Mode0_EnableIrq();                  //使能TIM3中断(模式0时只有一个中断)    
    //EnableNvic(ATIM3_IRQn, IrqLevel3, TRUE);  //ATIM3 开中断
    
    ATIM3_Mode0_Enable_Output(TRUE);            //ATIM3 端口输出使能
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


