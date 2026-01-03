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
uint32_t u32CaptureVal_0;
uint32_t u32CaptureVal_1;
volatile uint32_t u32CaptureVal_Result;
volatile uint32_t u32CaptureVal_OldResult;
uint32_t u32OverCntVal;
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
    
    ///< GTimer初始化
    App_GTimer_Init();      
    
    while (0 != STK_USERKEY_READ());          ///< 等待user key按下
    
    Gtim_Enable();   ///< 启动gtim运行
    
    while (1)
    {
        ;
    }
}


/**
 ******************************************************************************
 ** \brief  GTIMER中断服务函数
 **
 ** \return 无
 ******************************************************************************/
void GTim_IRQHandler(void)
{
    static uint32_t index=0;
    
    if (TRUE == Gtim_IsActiveFlag(GTIM_IT_FLAG_UI))
    {
        Gtim_ClearFlag(GTIM_IT_CLR_UI);//清除GTIM的溢出中断标志位  
        
        u32OverCntVal += 0x10000; 
    }
    
    if (TRUE == Gtim_IsActiveFlag(GTIM_IT_FLAG_CC1))
    {
        Gtim_ClearFlag(GTIM_IT_CLR_CC1);//清除GTIM的捕获比较0中断标志位 
        
        if (index == 0) //第一次检测到上升沿，起始计数
        {
            index = 1;
            
            u32CaptureVal_0 = Gtim_GetCompareCaptureReg(GTIM_COMPARE_CAPTURE_CH1);
            u32OverCntVal = 0;
            
            STK_LED_ON();     //点亮LED
        }
        else          //第二次检测到上升沿，结束计数,计算两次上升沿之间的值
        {
            u32CaptureVal_1 = Gtim_GetCompareCaptureReg(GTIM_COMPARE_CAPTURE_CH1);
            
            u32CaptureVal_Result = u32CaptureVal_1 + u32OverCntVal - u32CaptureVal_0;
            
            if(u32CaptureVal_Result > 0x80000)
            {
              u32CaptureVal_Result = 0;
            }
            
            u32CaptureVal_OldResult = u32CaptureVal_Result;
            
            u32OverCntVal = 0;
            
            STK_LED_OFF();     //熄灭LED
            
            index = 0;
        }
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
    stc_gpio_init_t stcChxGpioInit = {0};
    stc_gpio_init_t stcKEYGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);

    
    STK_LED_OFF();     //关闭LED
    ///< LED(PB04)端口初始化
    stcLEDGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcLEDGpioInit.u32Pin  = STK_LED_PIN;
    stcLEDGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcLEDGpioInit);
    
    ///< USER KEY(PB03)端口初始化
    stcKEYGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcKEYGpioInit.u32Pin  = STK_USERKEY_PIN;
    stcKEYGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcKEYGpioInit);
    
    ///< GTIM_CH1(PB02)端口初始化
    stcChxGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcChxGpioInit.u32Pin  = GPIO_PIN_02;
    stcChxGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcChxGpioInit);
    
    GPIO_PB02_AF_GTIM_CH1_SET();
}

/**
 ******************************************************************************
 ** \brief  初始化BTIMER0
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
    
    stcInitCfg.u32TaskMode      = GTIM_TASK_MODE_CONTINUOUS_COUNTER; //计数模式:连续计数
    stcInitCfg.u32WorkMode      = GTIM_WORK_MODE_PCLK;               //工作模式:定时器模式，计数时钟源来自PCLK
    stcInitCfg.u32Prescaler     = GTIM_COUNTER_CLK_DIV1;             //对计数时钟进行预除频
    stcInitCfg.u32ToggleEn      = GTIM_TOGGLE_DISABLE;               //TOG输出禁止
    //stcInitCfg.u32TriggerSource = GTIM_TRIGGER_SOURCE_ETR;         //触发源选择，此样例用于内部PCLK计数，不需要配置
    //stcInitCfg.u32ExInputPolarity = GTIM_ETR_POLARITY_NORMAL;      //外部输入极性选择，此样例用于内部PCLK计数，不需要配置
    stcInitCfg.u32AutoReloadVal = 0xFFFF;                            //自动重载寄存ARR赋值
    Gtim_Init(&stcInitCfg);
    
    Gtim_SetCompareCaptureMode(GTIM_COMPARE_CAPTURE_CH1, GTIM_COMPARE_CAPTURE_RISING);
    
    Gtim_ClearFlag((GTIM_IT_CLR_UI | GTIM_IT_CLR_CC1));   //清除溢出中断和捕获比较0中断标志位
    Gtim_EnableIT((GTIM_IT_UI | GTIM_IT_CC1));            //允许GTIM溢出中断    
    EnableNvic(GTIM_IRQn, IrqLevel3, TRUE); 
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
