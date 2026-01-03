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


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void App_Clock_Cfg(void);
void App_Timer3_Port_Cfg(void);
void App_Timer3_Cfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare);

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
    App_Clock_Cfg();                            //时钟初始化
    
    App_Timer3_Port_Cfg();                      //Atimer3 Port端口配置
    
    App_Timer3_Cfg(0x9000, 0x6000, 0x3000);     //Atimer3 配置:周期 0x9000; CH0/1/2通道A比较值0x6000; CH0/1/2通道B比较值0x3000
    
    ATIM3_Mode23_Run();                         //运行。
    
    //可添加其他其他操作，待准备好后使能PWM输出
    
    ATIM3_Mode23_Manual_PWM_Output_Enable(TRUE);//端口输出使能
    
    while (1);
    
    
    
}


/*******************************************************************************
 * ATIM3中断服务函数
 ******************************************************************************/
void ATim3_IRQHandler(void)
{
    static uint8_t i;
    
    //Atimer3 模式23 更新中断
    if(TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_UI))
    {
        if(0 == i)
        {
            STK_LED_ON();  //LED 引脚输出高电平
            
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0A, 0x3000); //设置CH0 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0B, 0x6000); //设置CH0 通道B比较值
            
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1A, 0x3000); //设置CH1 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1B, 0x6000); //设置CH1 通道B比较值
            
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2A, 0x3000); //设置CH2 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2B, 0x6000); //设置CH2 通道B比较值
            
            i++;
        }
        else if(1 == i)
        {
            STK_LED_OFF();  //LED 引脚输出低电平
            
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0A, 0x6000); //设置CH0 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0B, 0x3000); //设置CH0 通道B比较值

            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1A, 0x6000); //设置CH1 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1B, 0x3000); //设置CH1 通道B比较值
            
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2A, 0x6000); //设置CH2 通道A比较值
            ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2B, 0x3000); //设置CH2 通道B比较值
            
            i = 0;
        }
        
        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI);  //清中断标志
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
void App_Timer3_Port_Cfg(void)
{
    stc_gpio_init_t stcLEDGpioInit = {0};
    stc_gpio_init_t stcCHAGpioInit = {0};
    stc_gpio_init_t stcCHBGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);
    
    STK_LED_OFF();     //关闭LED
    ///< LED(PB04)端口初始化
    stcLEDGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcLEDGpioInit.u32Pin  = STK_LED_PIN;
    stcLEDGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcLEDGpioInit);
    
    ///< PD05(AIMT3_CH0A)/PD03(AIMT3_CH1A)/PD02(AIMT3_CH2A)端口初始化
    stcCHAGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHAGpioInit.u32Pin  = GPIO_PIN_02|GPIO_PIN_03|GPIO_PIN_05;
    stcCHAGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOD_Init(&stcCHAGpioInit);
    
    ///< PB00(AIMT3_CH0B)/PB01(AIMT3_CH1B)/PB02(AIMT3_CH2B)端口初始化
    stcCHBGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcCHBGpioInit.u32Pin  = GPIO_PIN_00|GPIO_PIN_01|GPIO_PIN_02;
    stcCHBGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcCHBGpioInit);
    
    GPIO_PD05_AF_ATIM3_CH0A_SET();
    GPIO_PB00_AF_ATIM3_CH0B_SET();
    GPIO_PD03_AF_ATIM3_CH1A_SET();
    GPIO_PB01_AF_ATIM3_CH1B_SET();
    GPIO_PD02_AF_ATIM3_CH2A_SET();
    GPIO_PB02_AF_ATIM3_CH2B_SET();
}

//Atimer3 配置
void App_Timer3_Cfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare)
{
    uint16_t                          u16CntValue;
    stc_atim3_mode23_cfg_t            stcAtim3BaseCfg  = {0};
    stc_atim3_m23_compare_cfg_t       stcAtim3ChxaPortCmpCfg = {0};
    stc_atim3_m23_compare_cfg_t       stcAtim3ChxbPortCmpCfg = {0};
    stc_atim3_m23_rcr_cfg_t           stcRcrCfg = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3); //ATIM3 外设时钟使能
    
    stcAtim3BaseCfg.u32WorkMode    = ATIM3_M23_M23CR_WORK_MODE_TRIANGULAR; //三角波模式
    stcAtim3BaseCfg.u32CountClkSel = ATIM3_M23_M23CR_CT_PCLK;              //定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS         = ATIM3_M23_M23CR_ATIM3CLK_PRS1;        //PCLK
    //stcAtim3BaseCfg.u32CntDir    = ATIM3_M23_M23CR_DIR_UP_CNT;           //向上计数，在三角波模式时只读
    stcAtim3BaseCfg.u32PWMTypeSel  = ATIM3_M23_M23CR_COMP_PWM_INDEPT;      //独立输出PWM
    stcAtim3BaseCfg.u32PWM2sSel    = ATIM3_M23_M23CR_PWM2S_COMPARE_SINGLE_POINT;//单点比较功能
    stcAtim3BaseCfg.u32ShotMode    = ATIM3_M23_M23CR_SHOT_CYCLE;           //循环计数
    stcAtim3BaseCfg.u32URSSel      = ATIM3_M23_M23CR_URS_OV_UND;           //上下溢更新
    ATIM3_Mode23_Init(&stcAtim3BaseCfg);    //ATIM3 的模式23功能初始化
    
    ATIM3_Mode23_ARRSet(u16Period);         //设置重载值,并使能缓存
    ATIM3_Mode23_ARR_Buffer_Enable(TRUE);
    
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0A, u16CHxACompare); //设置CH0 通道A比较值
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH0B, u16CHxBCompare); //设置CH0 通道B比较值
      
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1A, u16CHxACompare); //设置CH1 通道A比较值
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH1B, u16CHxBCompare); //设置CH1 通道B比较值
          
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2A, u16CHxACompare); //设置CH2 通道A比较值
    ATIM3_Mode23_Channel_Compare_Value_Set(ATIM3_COMPARE_CAPTURE_CH2B, u16CHxBCompare); //设置CH2 通道B比较值
    
    stcAtim3ChxaPortCmpCfg.u32CHxCmpCap      = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxaPortCmpCfg.u32CHxCmpModeCtrl = ATIM3_M23_FLTR_OCMxx_PWM_MODE_2;    //OCREFA输出控制OCMA:PWM模式2
    stcAtim3ChxaPortCmpCfg.u32CHxPolarity    = ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT; //正常输出
    stcAtim3ChxaPortCmpCfg.u32CHxCmpBufEn    = ATIM3_M23_CRCHx_BUFEx_ENABLE;       //A通道缓存控制
    stcAtim3ChxaPortCmpCfg.u32CHxCmpIntSel   = ATIM3_M23_M23CR_CISA_DISABLE_IRQ;   //A通道比较中断控制:无
            
    ATIM3_Mode23_PortOutput_CHxA_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH0, &stcAtim3ChxaPortCmpCfg);  //比较输出端口CH0A配置
    ATIM3_Mode23_PortOutput_CHxA_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH1, &stcAtim3ChxaPortCmpCfg);  //比较输出端口CH1A配置
    ATIM3_Mode23_PortOutput_CHxA_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH2, &stcAtim3ChxaPortCmpCfg);  //比较输出端口CH2A配置
    
    stcAtim3ChxbPortCmpCfg.u32CHxCmpCap      = ATIM3_M23_CRCHx_CSA_CSB_COMPARE;
    stcAtim3ChxbPortCmpCfg.u32CHxCmpModeCtrl = ATIM3_M23_FLTR_OCMxx_PWM_MODE_2;    //OCREFB输出控制OCMB:PWM模式2(PWM互补模式下也要设置，避免强制输出)
    stcAtim3ChxbPortCmpCfg.u32CHxPolarity    = ATIM3_M23_FLTR_CCPxx_NORMAL_IN_OUT; //正常输出
    stcAtim3ChxbPortCmpCfg.u32CHxCmpBufEn    = ATIM3_M23_CRCHx_BUFEx_ENABLE;       //B通道缓存控制使能
    stcAtim3ChxbPortCmpCfg.u32CHxCmpIntSel   = ATIM3_M23_CRCHx_CISBx_DISABLE_IRQ;  //B通道比较中断控制:无
            
    ATIM3_Mode23_PortOutput_CHxB_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH0, &stcAtim3ChxbPortCmpCfg);  //比较输出端口CH0B配置
    ATIM3_Mode23_PortOutput_CHxB_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH1, &stcAtim3ChxbPortCmpCfg);  //比较输出端口CH1B配置
    ATIM3_Mode23_PortOutput_CHxB_Cfg(ATIM3_M23_OUTPUT_CHANNEL_CH2, &stcAtim3ChxbPortCmpCfg);  //比较输出端口CH2B配置
    
    
    stcRcrCfg.u32EnOverFLowMask = ATIM3_M23_RCR_OVF_IRQ_EVT_CNT_MASK;     //屏蔽上溢重复计数
    stcRcrCfg.u32EnUnderFlowMask = ATIM3_M23_RCR_UND_IRQ_EVT_CNT_ENABLE;  //使能下溢重复计数
    stcRcrCfg.u32RepeatCountNum = 0;
    ATIM3_Mode23_SetRepeatPeriod(&stcRcrCfg);                             //间隔周期设置
    
    u16CntValue = 0;
    ATIM3_Mode23_Cnt16Set(u16CntValue);              //设置计数初值
    
    ATIM3_ClearAllIrqFlag();                         //清中断标志
    ATIM3_Mode23_EnableIrq(ATIM3_M23_INT_UI);        //使能ATIM3 UEV更新中断
    EnableNvic(ATIM3_IRQn, IrqLevel0, TRUE);         //ATIM3中断使能
}



/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


