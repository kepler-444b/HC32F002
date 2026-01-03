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
 * @brief  Source file for GTIMER example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "gtim.h"
#include "atim3.h"
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
  
static void App_GPIO_Init(void);
static void App_GTimer_Init(void);
static void App_Atimer3_Cfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare);

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
    ///< GPIO初始化
    App_GPIO_Init();
    
    ///< Gtimer初始化
    App_GTimer_Init(); 
    
    ///< Atimer3初始化
    App_Atimer3_Cfg(3000, 0, 0); //周期, 最长为0xFFFF ; 比较值A和比较值B设置为0 
    
    Gtim_Enable();           //启动gtim运行
    ATIM3_Mode23_Run();      //ATIM3运行。
    
    
    while (1)
    {
        ;
    }
}


/**
 ******************************************************************************
 ** \brief  Gtimer中断服务函数
 **
 ** \return 无
 ******************************************************************************/
void GTim_IRQHandler(void)
{
    static uint8_t i;
    
    if (TRUE == Gtim_IsActiveFlag(GTIM_IT_FLAG_UI))
    {
        if(0 == i)
        {
            STK_LED_ON();  //LED 引脚输出高电平
            
            i++;
        }
        else if(1 == i)
        {
            STK_LED_OFF();  //LED 引脚输出低电平
            
            i = 0;
        }
        
        Gtim_ClearFlag(GTIM_IT_CLR_UI);//清除GTIM的溢出中断标志位    
    }
}


/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return 无
 **
 ** 
 **
 ******************************************************************************/
static void App_GPIO_Init(void)
{
    stc_gpio_init_t stcLEDGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    
    STK_LED_OFF();     //关闭LED
    ///< LED(PB04)端口初始化
    stcLEDGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcLEDGpioInit.u32Pin  = STK_LED_PIN;
    stcLEDGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcLEDGpioInit);
}

/**
 ******************************************************************************
 ** \brief  初始化Gtimer
 **
 ** \return 无
 ******************************************************************************/
static void App_GTimer_Init(void)
{
    stc_gtim_init_t    stcInitCfg = {0};    

    //配置GTIM有效，BTIM3/4/5无效
    SYSCTRL_FuncDisable(SYSCTRL_FUNC_GTIMCONFIG);
    
    ///< 使能GTIM 外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_GTIM);
    
    stcInitCfg.u32TaskMode      = GTIM_TASK_MODE_CONTINUOUS_COUNTER; //连续计数模式
    stcInitCfg.u32WorkMode      = GTIM_WORK_MODE_EXTERNAL;           //工作模式: 计数器模式，计数时钟源来自TRS
    stcInitCfg.u32Prescaler     = GTIM_COUNTER_CLK_DIV1;             //对计数时钟进行预除频
    stcInitCfg.u32ToggleEn      = GTIM_TOGGLE_DISABLE;               //TOG输出禁止
    stcInitCfg.u32TriggerSource = GTIM_TRIGGER_SOURCE_ATIM3_TRGO;    //触发源选择，TRS触发源未来自ATIM3_TRGO
    stcInitCfg.u32ExInputPolarity = GTIM_ETR_POLARITY_NORMAL;        //外部ETP输入极性选择，此样例用于内部PCLK计数，不需要配置
    stcInitCfg.u32AutoReloadVal = 1000-1;                            //自动重载寄存ARR赋值
    Gtim_Init(&stcInitCfg);
    
    Gtim_ClearFlag(GTIM_IT_CLR_UI);       //清除溢出中断标志位
    Gtim_EnableIT(GTIM_IT_UI);            //允许GTIM溢出中断和触发中断    
    EnableNvic(GTIM_IRQn, IrqLevel3, TRUE); 
}

/**
 ******************************************************************************
 ** \brief  初始化ATIM3
 **
 ** \return 无
 ******************************************************************************/
void App_Atimer3_Cfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare)
{
    uint16_t                           u16CntValue;
    stc_atim3_mode23_cfg_t             stcAtim3BaseCfg  = {0};
    stc_atim3_m23_master_slave_cfg_t   stcMasterTrigCfg = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3);   //Atimer3 外设时钟使能
        
    stcAtim3BaseCfg.u32WorkMode    = ATIM3_M23_M23CR_WORK_MODE_SAWTOOTH; //锯齿波模式
    stcAtim3BaseCfg.u32CountClkSel = ATIM3_M23_M23CR_CT_PCLK;            //定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS         = ATIM3_M23_M23CR_ATIM3CLK_PRS1;      //PCLK
    stcAtim3BaseCfg.u32CntDir      = ATIM3_M23_M23CR_DIR_UP_CNT;         //向上计数，在三角波模式时只读
    stcAtim3BaseCfg.u32PWMTypeSel  = ATIM3_M23_M23CR_COMP_PWM_INDEPT;    //独立输出PWM
    stcAtim3BaseCfg.u32PWM2sSel    = ATIM3_M23_M23CR_PWM2S_COMPARE_SINGLE_POINT;//单点比较功能
    stcAtim3BaseCfg.u32ShotMode    = ATIM3_M23_M23CR_SHOT_CYCLE;         //循环计数
    stcAtim3BaseCfg.u32URSSel      = ATIM3_M23_M23CR_URS_OV_UND;         //上下溢更新
    
    ATIM3_Mode23_Init(&stcAtim3BaseCfg);             //ATIM3 的模式23功能初始化
    
    ATIM3_Mode23_ARRSet(u16Period);
    ATIM3_Mode23_ARR_Buffer_Enable(TRUE);            //设置重载值,并使能缓存
    
    u16CntValue = 0;
    ATIM3_Mode23_Cnt16Set(u16CntValue);              //设置计数初值
    
    ATIM3_Mode23_EnableIrq (ATIM3_M23_INT_UI);
    
    stcMasterTrigCfg.u32MasterSrc = ATIM3_M23_MSCR_MMS_TRIG_SOURCE_UEV; //主模式输出UEV更新
    ATIM3_Mode23_MasterSlave_Trig_Set(&stcMasterTrigCfg);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
