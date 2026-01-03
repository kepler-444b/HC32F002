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
 * @brief  Source file for LPUART example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "lpuart.h"
#include "lpm.h"
#include "gpio.h"
#include "sysctrl.h"

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
uint8_t u8TxData[2] = {0x00,0x55};
uint8_t u8RxData = 00;
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_LpUartPortCfg(void);
void App_LpUartCfg(void);
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
    ///< 端口配置
    App_LpUartPortCfg();
    
    ///< LPUART配置
    App_LpUartCfg();
    
    printf("\n\r@ LPUART Printf Example:\n\r\
    ==>> HC32F002 STK \n\r\
    ==>> www.xhsc.com.cn \n\r");
    
    while(1)
    {
        ;
    }
}

///< LPUART配置
void App_LpUartCfg(void)
{
    stc_lpuart_init_t  stcInit = {0};
    
    ///<外设模块时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_LPUART0);    
    
    ///<LPUART 初始化
    stcInit.u32StopBits    = LPUART_STOPBITS_1;            ///< 1停止位
    stcInit.u32Parity      = LPUART_PARITY_NONE;           ///< 无校验
    stcInit.u32FrameLength = LPUART_FRAME_LEN_8BITS;       ///< 帧长8位
    stcInit.u32TransMode   = LPUART_MODE_TX;               ///< 发送模式
    stcInit.stcBaudRate.u32SclkSrc  = LPUART_CLKSRC_PCLK;  ///< 传输时钟源
    stcInit.stcBaudRate.u32Sclk     = SYSCTRL_GetPCLK();   ///< PCLK获取
    stcInit.stcBaudRate.u32BaudRate = 9600;                ///< 波特率

    LPUART_Init(LPUART0, &stcInit);
     
}

///< 端口配置
void App_LpUartPortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA);
    
    ///< UART端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_02;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);
    
    GPIO_PA02_AF_LPUART0_TXD_SET();
}

PUTCHAR_PROTOTYPE
{
    LPUART_Transmit(LPUART0, (uint8_t*)(&ch), 1, 0xFFFFFF);
    return ch;
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


