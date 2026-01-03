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
//#define I2C_SLAVEWT 0xA0
//#define I2C_SLAVERD 0xA1
#define I2C_SLAVEADDR 0x50
#define READLEN   10
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
volatile uint8_t SendFlg   = 0;
volatile uint8_t Comm_flg  = 0;
volatile uint8_t u8SendLen = 0;
volatile uint8_t u8RecvLen = 0;
volatile uint8_t u8Addr    = 0x00;  //EEPROM地址字节
volatile uint8_t u8Senddata[10] = {0x02,0x34,0x56,0x78,0x90,0x11,0x22,0x33,0x44,0x55};
volatile uint8_t u8Recdata[10]  = {0x00};
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_I2cCfg(void);
void App_PortCfg(void);
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
    ///< IO端口配置
    App_PortCfg();
    
    ///< 等待 USER按键 按下
    while (0 != STK_USERKEY_READ());  
    
    ///< I2C 模块配置
    App_I2cCfg();
    
    ///< 使能I2C模块
    I2C_Enable(I2C);    

    ///< I2C总线发送START起始位，开始传输数据
    I2C_SetStart(I2C, I2C_START_ENABLE);
    
    while(1)
    {
        if(1 == Comm_flg)                            ///< 发送完，转接收信号
        {
            Comm_flg = 0;                            ///< 启动主机接收模式
            delay1ms(100);
            I2C_SetStop(I2C, I2C_STOP_ENABLE);   ///< 发送停止标志位
            I2C_SetStart(I2C, I2C_START_ENABLE); ///< 重新启动I2C
        }
    }  
}

///< I2C 模块配置
void App_I2cCfg(void)
{
    stc_i2c_master_init_cfg_t stcI2cCfg = {0};
    
    /* 第一步：开启I2C时钟门控  */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_I2C);
    
    /* 第二步：复位I2C模块 */
    SYSCTRL_PeriphReset(SYSCTRL_PERIRESET_I2C); 

    /* 第三步：I2C主机 初始化配置 */
    stcI2cCfg.u32Pclk = SYSCTRL_GetPCLK();      ///< 获取PCLK时钟
    stcI2cCfg.u32BaudRate = 200000;             ///< 100KHz
    I2C_MasterInit(I2C,&stcI2cCfg);         ///< 模块主机模式初始化
    
    /* 第四步： 中断开启 */
    I2C_ClearFlag_SI(I2C);                  ///< 开启中断前清除中断标志
    EnableNvic(I2C_IRQn, IrqLevel3, TRUE);      ///< 系统中断使能
}

///< IO端口配置
void App_PortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    
    ///< GPIOA 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PA);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_PB);
    
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
    
    ///< I2C_SCL(PA02)和I2C_SDA(PA01)端口初始化
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_OD;
    stcGpioInit.u32Pin  = GPIO_PIN_01 | GPIO_PIN_02;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOA_Init(&stcGpioInit);    
    GPIO_PA02_AF_I2C_SCL_SET();    ///< PA02复用到SCL
    GPIO_PA01_AF_I2C_SDA_SET();    ///< PA01复用到SDA
}

///< I2c中断函数
void I2c_IRQHandler(void)
{
    uint8_t u8State = I2C_GetBusState(I2C);
    
    switch(u8State)
    {
        // 主机发送模式和接收模式状态码
        case 0x08:      //已发送起始条件，将发送SLA+W    ***主机写：步骤1    ***主机读：步骤1
            I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
            I2C_SetStop(I2C, I2C_STOP_DISABLE);  //清除stop
            I2C_WriteByte(I2C, (I2C_SLAVEADDR<<1));     //写命令
            break;
        case 0x10:      //已发送重复起始条件                                 ***主机读：步骤4
            I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
            I2C_SetStop(I2C, I2C_STOP_DISABLE);  //清除stop
            if(0 == SendFlg)                         //判断当前发送SLA+W还是SLA+R
            {
                I2C_WriteByte(I2C, (I2C_SLAVEADDR<<1)); //写命令
            }
            else
            {
                I2C_WriteByte(I2C, ((I2C_SLAVEADDR<<1) | 0x01)); //读命令
            }
            break;
        case 0x18:      //已发送SLA+W,并接收到ACK        ***主机写：步骤2    ***主机读：步骤2
            I2C_WriteByte(I2C, u8Addr);          //EEPROM地址字节
            break;
        case 0x20:      //上一次发送SLA+W后，收到NACK
        case 0x38:      //上一次在SLA+读或写时丢失仲裁
        case 0x30:      //已发送I2Cx_DATA中的数据，收到NACK
        case 0x48:      //发送SLA+R后，收到一个NACK
            //异常处理
            I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
            I2C_SetStop(I2C, I2C_STOP_DISABLE);  //清除stop
            break;            
        case 0x58:      //已接收到最后一个数据，NACK已返回                   ***主机读：步骤7
            u8Recdata[u8RecvLen++] = I2C_ReadByte(I2C); //读最后一字节数据返回NACK后，读取数据并发送停止条件
            I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
            I2C_SetStop(I2C, I2C_STOP_ENABLE);   //发送stop
            break;                
        case 0x28:      //已发送数据，接收到ACK, 此处是已发送从机内存地址u8Addr并接收到ACK   
            if(SendFlg == 1) //读数据发送完地址字节后，重复起始条件          ***主机读：步骤3
            {
                I2C_SetStart(I2C, I2C_START_ENABLE);//发送起始条件
                I2C_SetStop(I2C, I2C_STOP_DISABLE); //清除stop
            }
            else       //发送数据                         ***主机写：步骤3
            {                
                if(10 <= u8SendLen)   //已发送10个数据     
                {
                    u8SendLen = 0;
                    Comm_flg  = 1;
                    SendFlg   = 1;
                    
                    //I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
                    I2C_SetStop(I2C, I2C_STOP_ENABLE);   //发送停止条件
                }else
                {
                    I2C_WriteByte(I2C, u8Senddata[u8SendLen++]); //发送数据
                }
            }   
            break;
        case 0x40:      //已发送SLA+R，并接收到ACK                           ***主机读：步骤5
            u8RecvLen = 0;
            if(1 < READLEN)
            {
                I2C_SetAck(I2C, I2C_ACK);            //读取数据超过1个字节才发送ACK
            }
            break;    
        case 0x50:      //已接收到数据字节，ACK返回                          ***主机读：步骤6
            u8Recdata[u8RecvLen++] = I2C_ReadByte(I2C); //读取数据
            I2C_SetStart(I2C, I2C_START_DISABLE);//清除start
            I2C_SetStop(I2C, I2C_STOP_DISABLE);  //清除stop
            if(u8RecvLen == READLEN - 1)             //准备接收最后一字节后返回NACK
            {
                I2C_SetAck(I2C, I2C_NACK);
            }
            break;
        default:
            break;
    } 
    
    I2C_ClearFlag_SI(I2C);                               //清除中断状态标志位，SI标志位清除必须放在中断的最后面
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


