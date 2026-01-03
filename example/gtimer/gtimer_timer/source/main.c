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

    ///< Gtimer初始化
    App_GTimer_Init();

    Gtim_Enable(); ///< 启动gtim运行

    while (1) {
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
    static uint8_t i;

    if (TRUE == Gtim_IsActiveFlag(GTIM_IT_FLAG_UI)) {
        if (0 == i) {
            STK_LED_ON(); // LED 引脚输出高电平

            i++;
        } else if (1 == i) {
            STK_LED_OFF(); // LED 引脚输出低电平

            i = 0;
        }

        Gtim_ClearFlag(GTIM_IT_CLR_UI); // 清除GTIM的溢出中断标志位
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
    stc_gpio_init_t stcGpioInit = {0};

    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);

    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    STK_LED_OFF(); // 关闭LED
}

/**
 ******************************************************************************
 ** \brief  初始化BTIMER0
 **
 ** \return 无
 ******************************************************************************/
static void App_GTimer_Init(void)
{
    stc_gtim_init_t stcInitCfg = {0};

    // 配置GTIM有效，BTIM3/4/5无效
    SYSCTRL_FuncDisable(SYSCTRL_FUNC_GTIMCONFIG);

    ///< 使能BTIM0 外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_GTIM);

    stcInitCfg.u32TaskMode  = GTIM_TASK_MODE_CONTINUOUS_COUNTER; // 连续计数模式
    stcInitCfg.u32WorkMode  = GTIM_WORK_MODE_PCLK;               // 工作模式:定时器模式，计数时钟源来自PCLK
    stcInitCfg.u32Prescaler = GTIM_COUNTER_CLK_DIV256;           // 对计数时钟进行预除频
    stcInitCfg.u32ToggleEn  = GTIM_TOGGLE_DISABLE;               // TOG输出禁止
    // stcInitCfg.u32TriggerSource = BTIM_TRIGGER_SOURCE_ETR;         //触发源选择，此样例用于内部PCLK计数，不需要配置
    // stcInitCfg.u32ExInputPolarity = GTIM_ETR_POLARITY_NORMAL;      //外部输入极性选择，此样例用于内部PCLK计数，不需要配置
    stcInitCfg.u32AutoReloadVal = 4000 - 1; // 自动重载寄存ARR赋值
    Gtim_Init(&stcInitCfg);

    Gtim_DisableCompareCaptureAll();

    Gtim_ClearFlag(GTIM_IT_CLR_UI); // 清除溢出中断标志位
    Gtim_EnableIT(GTIM_IT_UI);      // 允许GTIM溢出中断
    EnableNvic(GTIM_IRQn, IrqLevel3, TRUE);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
