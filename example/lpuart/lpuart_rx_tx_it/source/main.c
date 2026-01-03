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
#define RX_TX_FRAME_LEN 1u ///< 通信帧长度
uint8_t pu8RxTxBuf[RX_TX_FRAME_LEN] = {0};
uint8_t gu8RxCmdTxLen               = 0; ///< 接收命令(返回数据的字节长度)
uint8_t pu8TestData[2]              = {0x0A, 0x0D};
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

    ///< 起始发送
    LPUART_Transmit(LPUART0, pu8TestData, 2, 0xFFFFFF);

    LPUART_ClearIrq(LPUART0, LPUART_FLAG_ALL);
    LPUART_EnableIrq(LPUART0, LPUART_INT_RC);
    EnableNvic(LPUART0_IRQn, IrqLevel2, TRUE);

    while (1) {
        ;
    }
}

void LpUart0_IRQHandler(void)
{
    if (LPUART_GetFlag(LPUART0, LPUART_FLAG_RC)) {
        if (LPUART_GetFlag(LPUART0, LPUART_FLAG_PE | LPUART_FLAG_FE)) {
            ///< 错误处理……
            LPUART_ClearIrq(LPUART0, LPUART_FLAG_PE | LPUART_FLAG_FE);
        } else {
            LPUART_ReceiveIT(LPUART0, &gu8RxCmdTxLen);
            LPUART_ClearIrq(LPUART0, LPUART_FLAG_RC);
            LPUART_EnableIrq(LPUART0, LPUART_INT_TXE);
            LPUART_DisableIrq(LPUART0, LPUART_INT_RC);
        }
    }

    if (LPUART_GetFlag(LPUART0, LPUART_FLAG_TXE)) {
        LPUART_ClearIrq(LPUART0, LPUART_FLAG_TXE);
        if (gu8RxCmdTxLen) {
            LPUART_TransmitIT(LPUART0, &gu8RxCmdTxLen);
            gu8RxCmdTxLen--;
        } else {
            LPUART_EnableIrq(LPUART0, LPUART_INT_RC);
            LPUART_DisableIrq(LPUART0, LPUART_INT_TXE);
        }
    }
}

///< LPUART配置
void App_LpUartCfg(void)
{
    stc_lpuart_init_t stcInit = {0};

    ///< 外设模块时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_LPUART0);

    ///< LPUART 初始化
    stcInit.u32StopBits             = LPUART_STOPBITS_1;      ///< 1停止位
    stcInit.u32Parity               = LPUART_PARITY_NONE;     ///< 无校验
    stcInit.u32FrameLength          = LPUART_FRAME_LEN_8BITS; ///< 帧长8位
    stcInit.u32TransMode            = LPUART_MODE_TX_RX;      ///< 发送模式
    stcInit.stcBaudRate.u32SclkSrc  = LPUART_CLKSRC_PCLK;     ///< 传输时钟源
    stcInit.stcBaudRate.u32Sclk     = SYSCTRL_GetPCLK();      ///< PCLK获取
    stcInit.stcBaudRate.u32BaudRate = 9600;                   ///< 波特率

    LPUART_Init(LPUART0, &stcInit);
}

///< 端口配置
void App_LpUartPortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA);

    GPIO_PA02_SET();
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = GPIO_PIN_02;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);

    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_01;
    GPIOA_Init(&stcGpioInit);

    GPIO_PA02_AF_LPUART0_TXD_SET();
    GPIO_PA01_AF_LPUART0_RXD_SET();
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
