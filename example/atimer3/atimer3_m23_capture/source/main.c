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
#include "flash.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint32_t  Atim3_Capture_Value1,Atim3_Capure_Value2;
volatile int32_t   Atim3_Capture_Value;
volatile uint16_t  Atim3_Uev_Cnt;

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_Clock_Cfg(void);
void App_Atimer3_Port_Cfg(void);
void App_Timer3_Cfg(void);

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
    App_Clock_Cfg();        //时钟初始化
    
    App_Atimer3_Port_Cfg(); //Atimer3 Port端口配置
    
    App_Timer3_Cfg();       //Atimer3 配置
    
    ATIM3_Mode23_Run();     //运行。
    
    while (1)
    {
      
    }
}



/*******************************************************************************
 * ATIM3中断服务函数
 ******************************************************************************/
void ATim3_IRQHandler(void)
{
    static uint8_t i;
    
    //Timer3 捕获中断A
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_PWC_CA0))
    {
        if(0 == i)
        {
            Atim3_Capture_Value1 = ATIM3_Mode23_Channel_Capture_Value_Get(ATIM3_COMPARE_CAPTURE_CH0A);   //第一次读取捕获值
            Atim3_Uev_Cnt = 0;
            i++;
        }
        else 
        {
            Atim3_Capure_Value2 = ATIM3_Mode23_Channel_Capture_Value_Get(ATIM3_COMPARE_CAPTURE_CH0A);   //第二次读取捕获值
            Atim3_Capture_Value = Atim3_Uev_Cnt * 0xFFFF + Atim3_Capure_Value2 - Atim3_Capture_Value1;  //两次捕获之间的差值
            
            Atim3_Uev_Cnt = 0;
            
            i = 0;
        }
        
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_PWC_CA0);  //清中断标志
    }
    
    //timer3计数溢出中断
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_UI))
    {
        Atim3_Uev_Cnt++;
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI);
    }
}

//时钟初始化
void App_Clock_Cfg(void)
{
    stc_sysctrl_sysclk_source_init_t   stcSysClkSrc = {0};
    stc_sysctrl_clk_init_t             stcSysClk = {0};

    ///< 系统时钟源初始化
    stcSysClkSrc.u32SysClkSourceType = SYSCTRL_SYSCLK_SOURCE_TYPE_RCH;   //时钟源初始化需要操作RCH
        
    stcSysClkSrc.u32RCHState         = SYSCTRL_RCH_TRIM_48MHz |\
                                       SYSCTRL_RCH_DIV1;            //RCH 设置为1分频  -->48M
    SYSCTRL_SysClkSrcInit(&stcSysClkSrc);   //时钟源初始化
    
    ///< 系统时钟初始化
    stcSysClk.u32ClockType    = SYSCTRL_CLOCKTYPE_SYSCLK |\
                                SYSCTRL_CLOCKTYPE_HCLK   |\
                                SYSCTRL_CLOCKTYPE_PCLK;    //时钟初始化需要操作PLL，HCLK和PCLK
    stcSysClk.u32SysClkSource = SYSCTRL_SYSCLK_SOURCE_RCH; //RCH为系统时钟
    stcSysClk.u32HClkDiv      = SYSCTRL_SYSCLK_HCLK_PRS1;  //HCLK不分频
    stcSysClk.u32PClkDiv      = SYSCTRL_SYSCLK_PCLK_PRS1;  //PCLK不分频
    stcSysClk.u32WaitCycle    = SYSCTRL_FLASH_WAIT_CYCLE2; //设置flash 读等待为2个周期
    
    SYSCTRL_SysClkInit(&stcSysClk);     //时钟初始化
}


//Atimer3 Port端口配置
void App_Atimer3_Port_Cfg(void)
{
    stc_gpio_init_t stcCHAGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);
    
    
    ///< PD05(AIMT3_CH0A)/PD03(AIMT3_CH1A)/PD02(AIMT3_CH2A)端口初始化
    stcCHAGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHAGpioInit.u32Pin  = GPIO_PIN_05;
    stcCHAGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOD_Init(&stcCHAGpioInit);
    
    GPIO_PD05_AF_ATIM3_CH0A_SET();
}


//Atimer3 配置
void App_Timer3_Cfg(void)
{
    uint16_t                   u16ArrValue;
    uint16_t                   u16CntValue;
    stc_atim3_mode23_cfg_t     stcAtim3BaseCfg  = {0};
    stc_atim3_m23_input_cfg_t  stcAtim3CHxAInputCfg = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3); //ATIM3 外设时钟使能
    
    stcAtim3BaseCfg.u32WorkMode    = ATIM3_M1_M1CR_WORK_MODE_SAWTOOTH;//齿波模式
    stcAtim3BaseCfg.u32CountClkSel = ATIM3_M23_M23CR_CT_PCLK;         //定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS         = ATIM3_M23_M23CR_ATIM3CLK_PRS64;  //PCLK/64
    stcAtim3BaseCfg.u32CntDir      = ATIM3_M23_M23CR_DIR_UP_CNT;      //向上计数，在三角波模式时只读
    ATIM3_Mode23_Init(&stcAtim3BaseCfg);                              //ATIM3 的模式23功能初始化
        
    stcAtim3CHxAInputCfg.u32CHxCmpCap   = ATIM3_M23_CRCHx_CSA_CSB_CAPTURE;              //CH0A通道设置为捕获模式
    stcAtim3CHxAInputCfg.u32CHxCapSel   = ATIM3_M23_CRCHx_CRxCFx_CAPTURE_EDGE_RISE_FALL;//CH0A通道上升沿下降沿捕获都使能
    stcAtim3CHxAInputCfg.u32CHxInFlt    = ATIM3_M23_FLTR_FLTxx_THREE_PCLK_4;            //PCLK/4 3个连续有效
    stcAtim3CHxAInputCfg.u32CHxPolarity = ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT;           //正常输入输出
    ATIM3_Mode23_PortInput_CHxA_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH0, &stcAtim3CHxAInputCfg); //端口输入初始化配置
    
    u16ArrValue = 0xFFFF;
    ATIM3_Mode23_ARRSet(u16ArrValue);            //设置重载值,并使能缓存
    ATIM3_Mode23_ARR_Buffer_Enable(TRUE);
    
    u16CntValue = 0;
    ATIM3_Mode1_Cnt16Set(u16CntValue);           //设置计数初值
    
    ATIM3_ClearAllIrqFlag();                     //清中断标志
    ATIM3_Mode23_EnableIrq(ATIM3_M23_INT_CA0 | ATIM3_M23_INT_UI);   //使能ATIM3 CB0比较/捕获中断
    EnableNvic(ATIM3_IRQn, IrqLevel1, TRUE);     //ATIM3中断使能
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


