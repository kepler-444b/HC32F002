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
#define I2C_DEVADDR 0x50

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
uint8_t u8Senddata[10] = {0x12,0x34,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x99};
uint8_t u8Recdata[10]={0x00};

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
en_result_t I2C_MasterWriteData(I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len);//主发送函数
en_result_t I2C_MasterReadData(I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len); //主接收函数
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
    ///< 时钟配置
    //App_ClkCfg();
    
    ///< IO端口配置
    App_PortCfg();
    
    ///< 等待按键按下
    while (0 != STK_USERKEY_READ());
    
    ///< I2C 模块配置
    App_I2cCfg();
    
    ///< 使能I2C模块
    I2C_Enable(I2C);    
    
    ///< 启动I2C数据传输
    I2C_SetStart(I2C, I2C_START_ENABLE);
    I2C_MasterWriteData(I2C,u8Senddata,10);//主机发送10字节数据
    delay1ms(10);
    I2C_MasterReadData(I2C,u8Recdata,10);  //主机读取10字节数据
    while(1)
    {
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
    stcI2cCfg.u32BaudRate = 100000;             ///< 100KHz
    I2C_MasterInit(I2C,&stcI2cCfg);         ///< 模块主机模式初始化
}

/**
 ******************************************************************************
 ** \brief  主机接收函数
 **
 ** \param u8Addr从机内存地址，pu8Data读数据存放缓存，u32Len读数据长度
 **
 ** \retval 读数据是否成功
 **
 ******************************************************************************/
en_result_t I2C_MasterReadData(I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len)
{
    en_result_t enRet = Error;
    uint8_t u8i=0,u8State;

    ///< I2C总线发送START起始位
    I2C_SetStart(I2CX, I2C_START_ENABLE);    
    
    while(1)
    {
        while(0 == I2C_IsActiveFlag_SI(I2CX))          ///< 等待SI标志位置位
        {}
        u8State = I2C_GetBusState(I2CX);
        switch(u8State)
        {
            case 0x08:                                 ///< 已发送起始条件，将发送SLA+R                ***主机读：步骤1
                I2C_SetStart(I2CX, I2C_START_DISABLE); ///< 清除start
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1)|0x01);
                break;
            case 0x18:                                 ///< 已发送SLA+W,并接收到ACK(此样例中未用到)
                I2C_WriteByte(I2CX,0);                 ///< 发送内存地址
                break;
            case 0x28:                                 ///< 已发送数据，接收到ACK, 此处是已发送从机内存地址u8Addr并接收到ACK(此样例中未用到)
                I2C_SetStart(I2CX, I2C_START_ENABLE);   
                break;
            case 0x10:                                 ///< 已发送重复起始条件
                I2C_SetStart(I2CX, I2C_START_DISABLE);
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1)|0x01);///< 发送SLA+R，开始从从机读取数据
                break;
            case 0x40:                                 ///< 已发送SLA+R，并接收到ACK                   ***主机读：步骤2
                if(u32Len>1)
                {
                    I2C_SetAck(I2CX, I2C_ACK);         ///< 使能主机应答功能
                }
                break;
            case 0x50:                                 ///< 已接收数据字节，并已返回ACK信号            ***主机读：步骤3
                pu8Data[u8i++] = I2C_ReadByte(I2CX);
                if(u8i==u32Len-1)
                {
                    I2C_SetAck(I2CX, I2C_NACK);        ///< 已接收到倒数第二个字节，关闭ACK应答功能    ***主机读：步骤4
                }
                break;
            case 0x58:                                 ///< 已接收到最后一个数据，NACK已返回           ***主机读：步骤5
                pu8Data[u8i++] = I2C_ReadByte(I2CX);
                I2C_SetStop(I2CX, I2C_STOP_ENABLE);    ///< 发送停止条件
                break;
            case 0x38:                                 ///< 在发送地址或数据时，仲裁丢失
                I2C_SetStart(I2CX, I2C_START_ENABLE);  ///< 当总线空闲时发起起始条件
                break;
            case 0x48:                                 ///< 发送SLA+R后，收到一个NACK
                I2C_SetStop(I2CX, I2C_STOP_ENABLE);    ///< 发送停止条件
                I2C_SetStart(I2CX, I2C_START_ENABLE);  ///< 发送起始条件
                break;
            default:
                I2C_SetStart(I2CX, I2C_START_ENABLE);  ///< 其他错误状态，重新发送起始条件
                break;
        }
        I2C_ClearFlag_SI(I2CX);                        ///< 清除中断状态标志位
        if(u8i==u32Len)                                ///< 数据全部读取完成，跳出while循环
        {
                break;
        }
    }
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief  主机发送函数
 **
 ** \param u8Addr从机内存地址，pu8Data写数据，u32Len写数据长度
 **
 ** \retval 写数据是否成功
 **
 ******************************************************************************/
en_result_t I2C_MasterWriteData(I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len)
{
    en_result_t enRet = Error;
    uint8_t u8i=0, u8State, flag=FALSE;
    I2C_SetStart(I2CX, I2C_START_ENABLE);
    while(1)
    {
        while(0 == I2C_IsActiveFlag_SI(I2CX))
        {;}
        u8State = I2C_GetBusState(I2C);
        switch(u8State)
        {
            case 0x08:                               ///< 已发送起始条件                    ***主机写：步骤1
                I2C_SetStart(I2CX, I2C_START_DISABLE);
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1));   ///< 发送SLA+W
                break;
            case 0x18:                               ///< 已发送SLA+W，并接收到ACK          ***主机写：步骤2
            case 0x28:                               ///< 上一次发送数据后接收到ACK         ***主机写：步骤3
                I2C_WriteByte(I2CX,pu8Data[u8i++]);  ///< 继续发送数据
                break;
            case 0x20:                               ///< 上一次发送SLA+W后，收到NACK
            case 0x38:                               ///< 上一次在SLA+读或写时丢失仲裁
                I2C_SetStart(I2CX, I2C_START_ENABLE); ///< 当I2C总线空闲时发送起始条件
                break;
            case 0x30:                               ///< 已发送I2Cx_DATA中的数据，收到NACK，将传输一个STOP条件
                flag = TRUE;                         ///< 置标志位，传输TOP条件
                break;
            default:
                break;
        }
        if((u8i>u32Len) || (flag == TRUE))
        {
            I2C_SetStop(I2CX, I2C_STOP_ENABLE);      ///< 此顺序不能调换，出停止条件        ***主机写：步骤4
            I2C_ClearFlag_SI(I2CX);
            break;
        }
        I2C_ClearFlag_SI(I2CX);                          ///< 清除中断状态标志位
    }
    enRet = Ok;
    return enRet;    
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


