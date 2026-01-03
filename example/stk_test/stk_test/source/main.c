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
 * @brief  Source file for STK_TEST example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "sysctrl.h"
#include "gpio.h"
#include "lpuart.h"

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

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortCfg(void);
void App_LpUartPortCfg(void);
void App_LpUartCfg(void);

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return int return value, if needed
 **
 **
 ******************************************************************************/
int main(void)
{
    stc_sysctrl_chip_info_t stcChipInfo = {0};
    uint8_t u8Index;
    
    ///< MCU信息获取
    SYSCTRL_GetChipInfo(&stcChipInfo);
    
    ///< 配置端口
    App_PortCfg();
    App_LpUartPortCfg();
    
    ///< LPUART配置
    App_LpUartCfg();

    ///< 信息打印
    printf("@ Welcome to use the XHSC MCU !\n\r@ The Chip Info below are:\n\r");
    printf("* %s\n\r", stcChipInfo.pcProductNumber);
    printf("* UID - "); 
    for(u8Index=0; u8Index<10; u8Index++)
    {
        printf("%02X", stcChipInfo.pu8UID[u8Index]);
    }
    printf("\n\r");
    printf("* ARM Cortex-M0+ 32-bit, 48MHz\n\r");
    printf("* %dkB RAM, %dkB FLASH\n\r", stcChipInfo.u32RamSize/1000, stcChipInfo.u32FlashSize/1000);
    printf("* -40 ~ 105C, 1.7V ~ 5.5V\n\r");

    printf("\n\rPlease connect to \"http://www.xhsc.com.cn\" to obtain SDK!\n\r");

    while (1)
    {
        if(STK_USERKEY_READ())              ///< USER KEY 按键检测
        {
            STK_LED_ON();  delay1ms(100);
            STK_LED_OFF(); delay1ms(100);
        }
        else
        {
            STK_LED_ON();  delay1ms(500);
            STK_LED_OFF(); delay1ms(500);
        }
    }
}

///< 配置LED端口
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< GPIOB 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);

    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
        
    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    GPIOB_Init(&stcGpioInit);
}

///< LPUART配置
void App_LpUartCfg(void)
{
    stc_lpuart_init_t  stcInit = {0};
    
    ///<外设模块时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_LPUART0);    
    
    ///<LPUART 初始化
    stcInit.u32StopBits             = LPUART_STOPBITS_1;        ///< 1停止位
    stcInit.u32Parity               = LPUART_PARITY_NONE;       ///< 无校验
    stcInit.u32FrameLength          = LPUART_FRAME_LEN_8BITS;   ///< 帧长8位
    stcInit.u32TransMode            = LPUART_MODE_TX;           ///< 发送模式
    stcInit.stcBaudRate.u32SclkSrc  = LPUART_CLKSRC_PCLK;       ///< 传输时钟源
    stcInit.stcBaudRate.u32Sclk     = SYSCTRL_GetPCLK();        ///< PCLK获取
    stcInit.stcBaudRate.u32BaudRate = 9600;                     ///< 波特率

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

///< 串口打印重定向
PUTCHAR_PROTOTYPE
{
    LPUART_Transmit(LPUART0, (uint8_t*)(&ch), 1, 0xFFFFFF);
    return ch;
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



