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
 * @brief  Source file for I2C example
 *
 * @author MADS Team 
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "i2c.h"
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

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
uint8_t u8Recdata[10]={0x00};
uint8_t u8SendLen=0;
uint8_t u8RecvLen=0;
uint8_t u8State = 0;
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_ClkCfg(void);
void App_PortCfg(void);
void App_I2cCfg(void);
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
    ///< 时钟配置，此样例使用系统默认4MHz，不需要配置
    //App_ClkCfg();
    
    ///< IO端口配置
    App_PortCfg();
    
    ///< 等待按键按下
    while (0 != STK_USERKEY_READ());
    
    ///< I2C 模块配置
    App_I2cCfg();
    
    ///< 使能I2C模块
    I2C_Enable(I2C);    
        
    ///< 启动I2c数据传输
    I2C_SetAck(I2C, I2C_ACK);            ///< ACK打开
    
    while(1)
    {
        ;
    }
}

///< 时钟配置
void App_ClkCfg(void)
{    
    ;
}

///< IO端口配置
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< 开启 PA端口 时钟门控
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    
    ///< LED端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin  = STK_LED_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    STK_LED_OFF();                 ///< 关闭LED
    
    ///< USER KEY端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin  = STK_USERKEY_PIN;
    stcGpioInit.u32Pull = GPIO_PULL_NONE;
    GPIOB_Init(&stcGpioInit);
    
    ///< I2C_SCL(PA02)和I2C_SDA(PA01)端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_OD;
    stcGpioInit.u32Pin  = GPIO_PIN_01 | GPIO_PIN_02;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);    
    GPIO_PA02_AF_I2C_SCL_SET();    ///< PA02复用到SCL
    GPIO_PA01_AF_I2C_SDA_SET();    ///< PA01复用到SDA
}

///< I2C 模块配置
void App_I2cCfg(void)
{
    stc_i2c_slave_init_cfg_t stcI2cCfg = {0};
    
    /* 第一步：开启I2C时钟门控  */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_I2C);    
    
    /* 第二步：复位I2C模块 */
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_I2C);  
        
    /* 第三步：I2C从机 初始化配置 */
    stcI2cCfg.u8SlaveAddr0 = 0x50;                   ///< 从地址0，主模式无效
    stcI2cCfg.u8SlaveAddr1 = 0x00;                   ///< 从地址1，主模式无效
    stcI2cCfg.u8SlaveAddr2 = 0x00;                   ///< 从地址2，主模式无效
    stcI2cCfg.u32BroadcastEn = I2C_BROADCAST_DISABLE;///< 广播地址应答使能关闭，主模式无效
    stcI2cCfg.u32Filter = I2C_FILTER_SIMPLE;         ///< 高级滤波，更高的抗干扰性能
    I2C_SlaveInit(I2C, &stcI2cCfg);              ///< 模块初始化
    
    /* 第四步： 中断开启 */
    I2C_ClearFlag_SI(I2C);                       ///< 开启中断前清除中断标志
    EnableNvic(I2C_IRQn,IrqLevel3,TRUE);             ///< 使能NVIC对应中断位    
}

///< I2c0中断函数
void I2c_IRQHandler(void)
{
    u8State = I2C_GetBusState(I2C);
    switch(u8State)
    {
        case 0x60:         //已接收到(与自身匹配的)SLA+W；已接收ACK                 ***从机读：步骤1
        case 0x70:         //已接收通用调用地址（0x00）；已接收ACK
        case 0xa0:         //接收到停止条件或重复起始条件
            u8RecvLen = 0;
            break;
        case 0x68:         //主控时在SLA+读写丢失仲裁；已接收自身的SLA+W；已返回ACK
        case 0x78:         //主控时在SLA+读写中丢失仲裁；已接收通用调用地址；已返回ACK
        case 0x88:         //前一次寻址使用自身从地址；已接收数据字节；已返回非ACK
            break;
        case 0x80:         //前一次寻址使用自身从地址；已接收数据字节；已返回ACK    ***从机读：步骤2
        case 0x98:         //前一次寻址使用通用调用地址；已接收数据；已返回非ACK
        case 0x90:         //前一次寻址使用通用调用地址；已接收数据；已返回ACK
            u8Recdata[u8RecvLen++] = I2C_ReadByte(I2C);//接收数据
            break;        
        case 0xa8:         //已接收自身的SLA+R；已返回ACK,将要发出数据并将收到ACK位                     ***从机写：步骤1
        case 0xb0:         //当主控时在SLA+读写中丢失仲裁；已接收自身SLA+R；已返回ACK
            I2C_WriteByte(I2C,u8Recdata[u8SendLen++]); //发送数据首字节
            break;
        case 0xc8:        //装入的数据字节已被发送；已接收ACK
        case 0xb8:        //已发送数据；已接收ACK                                                       ***从机写：步骤2
            I2C_WriteByte(I2C,u8Recdata[u8SendLen++]);
            break;
        case 0xc0:        //发送数据，接收非ACK                                                         ***从机写：步骤3
            break;
    }
    
    I2C_ClearFlag_SI(I2C);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


