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
//从机相关数据
const    uint8_t u8Comp[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
volatile uint8_t u8SlaveBuf[10] = {0};
uint8_t u8Index=0;
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
    ///< 端口初始化
    App_GpioInit();
    
    ///< USER 按下启动通信
    while (0 != STK_USERKEY_READ());  
    
    ///< 从机SPI初始化
    App_SPIInit(); 
    
    ///< 从机SPI使能
    SPI_Enable(SPI); 
    
    ///< 接收数据
    for (u8Index=0; u8Index<10; u8Index++)
    {
        while(FALSE == SPI_IsActiveFlag(SPI, SPI_FLAG_RXNE));  ///< 如果接收缓存为空，则一直等待
        u8SlaveBuf[u8Index] = SPI_ReceiveData(SPI);
        SPI_ClearFlag(SPI, SPI_CLEAR_RXNE);
    }
    
    ///< 判断接收数据是否正确
    for (u8Index=0; u8Index<10; u8Index++)
    {
        if(u8SlaveBuf[u8Index] != u8Comp[u8Index])
        {
            break;
        }
    }
    if (u8Index == 10)  ///< 如果接收数据全部正确，LED亮
    {
        STK_LED_ON();
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
    
    /**************************配置从机SPI端口********************************/
    ///< 从机 SPI0_CS(PA03)端口初始化，上拉输入
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_03;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);    
    GPIO_PA03_AF_SPI_CS_SET();
    
    ///< 从机 SPI0_SCK(PD04)、SPI_MOSI(PD06)端口初始化，上拉输入
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_04 | GPIO_PIN_06;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOD_Init(&stcGpioInit);    
    GPIO_PD04_AF_SPI_SCK_SET();
    GPIO_PD06_AF_SPI_MOSI_SET();      
}

/**
 ******************************************************************************
 ** \brief  初始化 从机 SPI
 **
 ** \return 无
 ******************************************************************************/
static void App_SPIInit(void)
{

    stc_spi_init_t  SpiInitStruct={0};    
    
    ///< 打开外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_SPI);
    
    ///<复位模块
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_SPI);
    
    //SPI0模块配置：主机
    SpiInitStruct.u32TransferDirection = SPI_SIMPLE_RX;            //单工只收通讯
    SpiInitStruct.u32Mode              = SPI_MODE_SLAVE;           //从机模式
    SpiInitStruct.u32DataWidth         = SPI_DATAWIDTH_8BIT;       //8bit数据宽度
    SpiInitStruct.u32CPOL              = SPI_CLOCK_POLARITY_LOW;   //待机时低电平
    SpiInitStruct.u32CPHA              = SPI_CLOCK_PHASE_1EDGE;    //第一个边沿采样(第二个边沿移位)
    SpiInitStruct.u32NSS               = SPI_NSS_HARD_INPUT;       //NSS信号由IO管脚输入
    //SpiInitStruct.u32BaudRate        = SPI_BAUDRATE_PCLK_DIV16;  //pclk/16    从机不需要配置
    SpiInitStruct.u32BitOrder          = SPI_MSB_FIRST;            //最高有效位MSB收发在前
    Spi_Init(SPI, &SpiInitStruct);
      
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


