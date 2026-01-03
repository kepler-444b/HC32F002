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
static volatile uint8_t u8TrimTestFlag   = 0;
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
        ///< 判断是否收到自动Trim结束标志位
        if (TRUE == CTRIM_IsActiveFlag(CTRIM, CTRIM_FLAG_END))
        {
            CTRIM_ClearFlag(CTRIM, CTRIM_CLEAR_FLAG_END);  ///< 清除标志位
            
            ///< 点亮LED，告诉用户校验成功
            STK_LED_ON();
        }
    }    
}

//端口相关配置
void App_GpioCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< 开启GPIO外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PC);
    
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
    
    ///< CTRIM_ETR(PB05)端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_05;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);   
    GPIO_PB05_AF_CTRIM_ETR_TOG_SET();
    
    ///< 时钟输出端口初始化，PC04输出RCH
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_04;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOC_Init(&stcGpioInit);    
    GPIO_PC04_TCLK_OUT_RCH_DIV1();
}

static void App_TrimCalInit(void)
{
    stc_ctrim_cali_init_t stcCfg = {0};

    /* 第一步：开启CTRIM 外设时钟 */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_CTRIM);
    
    /* 第二步：复位CTRIM模块 */
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_CTRIM);
     
    /* 第三步：TRIM 初始化配置 */
    stcCfg.u32Mode            = CTRIM_MODE_RCH_AUTO_TRIM;
    stcCfg.u32AccurateClock   = CTRIM_ACCURATE_CLOCK_ETR;
    stcCfg.u32RefClockDiv     = CTRIM_REF_CLOCK_DIV2;  ///< GCLK频率为1000Hz
    stcCfg.u32OneShot         = CTRIM_ONE_SHOT_SINGLE;  
    stcCfg.u32InitialStep     = CTRIM_INITIAL_STEP_32;  
    //stcCfg.u32EtrFilter     = CTRIM_ETR_CLOCK_FILTER_NONE;  //未使用外部输入信号，可以不配置此值
    stcCfg.u16ReloadVal       = 48000-1;  ///< 1个GCLK周期里计48000个数，RCH理论校验频率=1000*48000=48MHz
    stcCfg.u8CountErrorLimit  = 240;      ///< 校准精度设置为0.5%，FLIM=48000*0.5%=240
    CTRIM_CaliInit(CTRIM, &stcCfg);
    
    /* 第四步：清除所有标志位 */
    CTRIM_ClearFlag_ALL(CTRIM);
    
    /* 第五步：使能中断，此应用不用中断，可以不配置 */
    //CTRIM_EnableIT(CTRIM, CTRIM_IT_END);        ///< 打开TRIM中断使能    
    //EnableNvic(CTRIM_FAULT_IRQn, IrqLevel3, TRUE);  ///< 使能并配置TRIM 系统中断
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


