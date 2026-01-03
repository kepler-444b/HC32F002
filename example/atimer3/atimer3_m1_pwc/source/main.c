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
#include "flash.h"
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
volatile uint32_t u32PwcCapValue;
volatile uint16_t u16ATIM3_CntValue;

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_Clock_Cfg(void);
void App_Atimer3_Port_Cfg(void);
void App_Atimer3_Cfg(void);

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
    
    App_Atimer3_Port_Cfg(); //ATIM3 Port端口配置
    
    App_Atimer3_Cfg();      //ATIM3 配置;
        
    ATIM3_Mode1_Run();      //ATIM3 运行。
    
    while (1);
}

/*******************************************************************************
 * ATIM3中断服务函数
 ******************************************************************************/
void ATim3_IRQHandler(void)
{
    static uint16_t u16ATIM3_OverFlowCnt;
    
    static uint16_t u16ATIM3_CapValue;
     
    //Atimer3 模式1 计数溢出中断
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_UI))
    {
        u16ATIM3_OverFlowCnt++;                   //计数脉宽测量开始边沿到结束边沿过程中timer的溢出次数
        
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI); //清除中断标志
    }

    //Atimer3 模式1 PWC脉宽测量中断
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_PWC_CA0))
    {
        u16ATIM3_CntValue = ATIM3_Mode1_Cnt16Get();         //读取当前计数值
        u16ATIM3_CapValue = ATIM3_Mode1_PWC_CapValueGet();  //读取脉宽测量值
        
        u32PwcCapValue = u16ATIM3_OverFlowCnt*0x10000+ u16ATIM3_CapValue;
        
        u16ATIM3_OverFlowCnt = 0;
        
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_PWC_CA0); //清除中断标志
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
    
    
    ///< PD05(AIMT3_CH0A)端口初始化
    stcCHAGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHAGpioInit.u32Pin  = GPIO_PIN_05;
    stcCHAGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOD_Init(&stcCHAGpioInit);
    
    GPIO_PD05_AF_ATIM3_CH0A_SET();
}

//Atimer3 配置
void App_Atimer3_Cfg(void)
{
    uint16_t                     u16CntValue;
    stc_atim3_mode1_cfg_t        stcAtim3BaseCfg  = {0};
    stc_atim3_pwc_input_cfg_t    stcAtim3PwcInCfg = {0};

    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3); //ATIM3 外设时钟使能
    
    stcAtim3BaseCfg.u32WorkMode      = ATIM3_M1_M1CR_WORK_MODE_PWC;  //定时器模式
    stcAtim3BaseCfg.u32CountClkSel   = ATIM3_M1_M1CR_CT_PCLK;        //定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS           = ATIM3_M1_M1CR_ATIM3CLK_PRS4;  //PCLK/4
    stcAtim3BaseCfg.u32ShotMode      = ATIM3_M1_M1CR_SHOT_CYCLE;     //PWC循环检测
    ATIM3_Mode1_Init(&stcAtim3BaseCfg);                              //TIM3 的模式1功能初始化
    
    stcAtim3PwcInCfg.u32TsSel  = ATIM3_M1_MSCR_TS_CH0A_FILTER;         //PWC输入选择 CHA
    stcAtim3PwcInCfg.u32IA0Sel = ATIM3_M1_MSCR_IA0S_CH0A;              //CHA选择IA0
    stcAtim3PwcInCfg.u32FltIA0 = ATIM3_M1_FLTR_FLTA0_B0_ET_3_PCLKDIV4; //PCLK/4 3个连续有效
    //stcAtim3PwcInCfg.u32IB0Sel = ATIM3_M1_MSCR_IB0S_CH0B;            //CHB选择IB0
    //stcAtim3PwcInCfg.u32FltIB0 = ATIM3_M1_FLTR_FLTA0_B0_ET_3_PCLKDIV4;//PCLK/4 3个连续有效
    ATIM3_Mode1_Input_Cfg(&stcAtim3PwcInCfg);                          //PWC输入设置
                     
    ATIM3_Mode1_PWC_Edge_Sel(ATIM3_M1_M1CR_DETECT_EDGE_FALL_TO_FALL);//下降沿到下降沿脉宽测量
      
    u16CntValue = 0;
    ATIM3_Mode1_Cnt16Set(u16CntValue);                              //设置计数初值
    
    ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI |ATIM3_INT_CLR_PWC_CA0);    //清Uev中断、捕捉中断标志
    ATIM3_Mode1_EnableIrq(ATIM3_M1_INT_UI | ATIM3_M1_INT_CA0);      //使能TIM3溢出中断
    EnableNvic(ATIM3_IRQn, IrqLevel3, TRUE);                        //TIM3中断使能
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


