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
 * @brief  Source file for BTIMER example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "btim.h"
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
static void App_GPIO_Init(void);
static void App_Btimer_Init(void);

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
    ///< GPIO初始化
    App_GPIO_Init();
    
    ///< Btimer3初始化
    App_Btimer_Init();      
    
    Btim_Enable(BTIM3);   ///< 启动btim3运行
    
    while (1)
    {
        ;
    }
}


/**
 ******************************************************************************
 ** \brief  BTIMER3 中断服务函数
 **
 ** \return 无
 ******************************************************************************/
void BTim3_IRQHandler(void)
{
    static uint8_t i;
    
    if (TRUE == Btim_IsActiveFlag(BTIM3, BTIM_IT_FLAG_UI))
    {
        if(0 == i)
        {
            STK_LED_ON();  //LED 引脚输出高电平
            
            i++;
        }
        else
        {
            STK_LED_OFF();  //LED 引脚输出低电平
            
            i = 0;
        }
        
        Btim_ClearFlag(BTIM3, BTIM_IT_CLR_UI);//清除GTIM的溢出中断标志位    
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
    stc_gpio_init_t stcETRGpioInit = {0};
    stc_gpio_init_t stcTogGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);
    
    STK_LED_OFF();     //关闭LED
    ///< LED(PB04)端口初始化
    stcLEDGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcLEDGpioInit.u32Pin  = STK_LED_PIN;
    stcLEDGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcLEDGpioInit);
    
    
    ///< ETR(PD06)端口初始化
    stcETRGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcETRGpioInit.u32Pin  = GPIO_PIN_06;
    stcETRGpioInit.u32Pull = GPIO_PULL_DOWN;
    GPIOD_Init(&stcETRGpioInit);
    
    GPIO_PD06_AF_GTIM_ETR_SET();
    
    
    ///< BTIM3_TOGP(PA01)和BTIM3_TOGN(PA02)端口初始化
    stcTogGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcTogGpioInit.u32Pin  = GPIO_PIN_01 | GPIO_PIN_02;
    stcTogGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOA_Init(&stcTogGpioInit);
    
    GPIO_PA01_AF_GTIM_TOGP_SET();
    GPIO_PA02_AF_GTIM_TOGN_SET();
    
}

/**
 ******************************************************************************
 ** \brief  初始化BTIMER3
 **
 ** \return 无
 ******************************************************************************/
static void App_Btimer_Init(void)
{
    stc_btim_init_t    stcInitCfg = {0};    

    //配置GTIM无效，BTIM3/4/5有效
    SYSCTRL_FuncEnable(SYSCTRL_FUNC_GTIMCONFIG);

    //使能BTIM3/4/5 外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_GTIM);
    
    stcInitCfg.u32TaskMode        = BTIM_TASK_MODE_CONTINUOUS_COUNTER; //计数模式：连续计数
    stcInitCfg.u32WorkMode        = BTIM_WORK_MODE_GATE;               //工作模式:定时器模式，计数时钟源来自PCLK
    stcInitCfg.u32Prescaler       = BTIM_COUNTER_CLK_DIV256;           //对计数时钟进行预除频
    stcInitCfg.u32ToggleEn        = BTIM_TOGGLE_ENABLE;                //TOG输出使能，TOGP和TOGN输出相位相反的信号
    stcInitCfg.u32TriggerSource   = BTIM_TRIGGER_SOURCE_ETR;           //触发源选择，使用外部ETR输入作为GATE门控
    stcInitCfg.u32ExInputPolarity = BTIM_ETR_POLARITY_INVERTED;        //外部输入极性选择，极性不翻转
    stcInitCfg.u32AutoReloadVal   = 4000-1;                            //自动重载寄存ARR赋值
    Btim_Init(BTIM3, &stcInitCfg);
    
    Btim_ClearFlag(BTIM3, BTIM_IT_CLR_UI); //清除溢出中断标志位
    Btim_EnableIT(BTIM3, BTIM_IT_UI);      //允许BTIM3溢出中断    
    EnableNvic(BTIM3_4_5_IRQn, IrqLevel3, TRUE); 
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
