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
 * @brief  Source file for SPI example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "spi.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
//主机相关数据
const uint8_t u8Cmp[10]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
volatile uint8_t u8MasterRxBuf[10] = {0};
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
static void App_GpioInit(void);
static void App_SPIInit(void);

/**
******************************************************************************
    ** \brief  主函数
    ** 
    ** @param  无
    ** \retval 无
    **
******************************************************************************/ 
int main(void)
{
    uint8_t index=0;

    ///< 端口初始化
    App_GpioInit();
    
    ///< USER 按下启动通信
    while (0 != STK_USERKEY_READ()); 
    
    ///< 主机SPI初始化
    App_SPIInit(); 
    
    ///< 主机SPI0使能
    SPI_Enable(SPI);   
    
    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_ENABLE);      //NSS片选，开始通讯

    ///< 主机读取从机数据
    for (index=0; index<10; index++)
    {
        //写数据，是为了主机能启动传输
        while(FALSE == SPI_IsActiveFlag(SPI, SPI_FLAG_TXE));  //当前有数据在传输，则等待
        SPI_TransmitData(SPI, 0x55);  //随便写入一个数据，让主机启动传输
        //接收数据
        while(FALSE == SPI_IsActiveFlag(SPI, SPI_FLAG_RXNE));  //接收缓存为空，则等待
        u8MasterRxBuf[index] = SPI_ReceiveData(SPI);
        SPI_ClearFlag(SPI, SPI_CLEAR_RXNE);
    }    
    
    delay1ms(10);  //写入发送数据后，不能立马取消片选，延时，等待总线上数据传输完成，
    SPI_MasterNSSOutput(SPI, SPI_NSS_CONFIG_DISABLE); //取消片选，结束通信
    
    //判断数据接收是否正确
    for (index=0; index<10; index++)
    {
        if (u8Cmp[index] != u8MasterRxBuf[index])
        {
            break;
        }
    }
    if (index == 10)  //数据全部接收正确
    {
        STK_LED_ON(); //LED亮
    }
    
    while(1)
    {
        ;
    }        
}

/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return 无
 ******************************************************************************/
static void App_GpioInit(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PD);
    
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
    
    /**************************配置主机SPI0端口********************************/    
    ///< 主机 SPI0_CS(PA03)端口初始化，推挽输出
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_DRV_H;
    stcGpioInit.u32Pin  = GPIO_PIN_03;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOA_Init(&stcGpioInit);
    
    GPIO_PA03_AF_SPI_CS_SET();
    
    ///< 主机 SPI0_SCK(PD04)端口初始化，推挽输出
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_DRV_H;
    stcGpioInit.u32Pin  = GPIO_PIN_04;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOD_Init(&stcGpioInit);
    
    GPIO_PD04_AF_SPI_SCK_SET();
    //GPIO_PB05_AF_SPI0_MOSI_SET();
    
    ///< 主机 SPI_MISO(PD05)端口初始化，上拉输入
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_05;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOD_Init(&stcGpioInit);
    
    GPIO_PD05_AF_SPI_MISO_SET();     
}

/**
 ******************************************************************************
 ** \brief  初始化 主机 SPI
 **
 ** \return 无
 ******************************************************************************/
static void App_SPIInit(void)
{
    stc_spi_init_t  SpiInitStruct={0};    
    
    /* 第一步：开启SPI时钟门控  */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_SPI);
    
    /* 第二步：复位SPI模块 */
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_SPI);
    
    /* 第三步：SPI模块配置：从机 */
    SpiInitStruct.u32TransferDirection = SPI_SIMPLE_RX;            ///< 单工只收模式
    SpiInitStruct.u32Mode              = SPI_MODE_MASTER;          ///< 主机模式
    SpiInitStruct.u32DataWidth         = SPI_DATAWIDTH_8BIT;       ///< 8bit数据宽度
    SpiInitStruct.u32CPOL              = SPI_CLOCK_POLARITY_LOW;   ///< 待机时低电平
    SpiInitStruct.u32CPHA              = SPI_CLOCK_PHASE_1EDGE;    ///< 第一个边沿采样(第二个边沿移位)
    SpiInitStruct.u32NSS               = SPI_NSS_HARD_OUTPUT;      ///< NSS信号由IO管脚输出
    SpiInitStruct.u32BaudRate          = SPI_BAUDRATE_PCLK_DIV128; ///< pclk/128
    SpiInitStruct.u32BitOrder          = SPI_MSB_FIRST;            ///< 最高有效位MSB收发在前
    Spi_Init(SPI, &SpiInitStruct);
      
    /* 第四步： 清除所有中断标志位 */
    SPI_ClearFlag_ALL(SPI);              ///< 清除所有中断标志位
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


