#include "sys.h"
#include "xunSub.h"
#include "RelayAPP.h"

Pro_flag_s MCU_flag;

// 获取硬件的UID，共80位占用12个字节
u16 Get_UID(u8 *buf)
{
    uint32_t temp;
    uint32_t addr = 0x00100980;
    u16 i;
    for (i = 0; i < 3; i++)
    {
        temp = *((volatile uint32_t *)(addr + i * 4));
        buf[i * 4] = (u8)temp;
        buf[i * 4 + 1] = (u8)(temp >> 8);
        buf[i * 4 + 2] = (u8)(temp >> 16);
        buf[i * 4 + 3] = (u8)(temp >> 24);
    }
    return crc_chk_value(buf, 12);
}

/****************************************
校验和为0x64
*****************************************/
u8 GetCheckCodeX(u8 *cmd, u8 BoxLen)
{
    u8 i, res;
    res = 0x64;
    for (i = 0; i < BoxLen; i++)
        res -= cmd[i];
    return res;
}
/****************************************
校验和
*****************************************/
u8 GetCheckHe(u8 *cmd, u8 BoxLen)
{
    u8 i, res = 0;

    for (i = 0; i < BoxLen; i++)
        res += cmd[i];
    return res;
}
uint8_t calcrc_data(uint8_t *rxbuf, uint8_t len)
{
    uint8_t i, sum = 0;
    for (i = 0; i < len; i++)
        sum = sum + rxbuf[i];
    return (0xff - sum + 1);
}

/****************************************
modbus 校验双字节
*****************************************/
unsigned int crc_chk_value(unsigned char *data_value, u16 length)
{
    unsigned int crc_value = 0xFFFF;
    int i;
    while (length--)
    {
        crc_value ^= *data_value++;
        for (i = 0; i < 8; i++)
        {
            if (crc_value & 0x0001)
            {
                crc_value = (crc_value >> 1) ^ 0xa001;
            }
            else
            {
                crc_value = crc_value >> 1;
            }
        }
    }
    return (crc_value);
}
/*异或+校验和*/
u16 BCC_CheckSum(u8 *buf, u8 len)
{
    u8 i;
    u8 checksum = 0;
    u8 checksumhe = 0;
    u16 re = 0;
    for (i = 0; i < len; i++)
    {

        checksum ^= buf[i];
        checksumhe = checksumhe + buf[i];
    }
    re = checksum;
    re = (re << 8) + checksumhe;
    return re;
}

// 时钟初始化配置
/*
void App_ClkInit(u8 pinlv)
{
    volatile uint32_t u32Val = 0;
    //Clk_SwitchTo(ClkRCL);
        //Clk_SetRCHFreq(ClkFreq4Mhz);
        //Clk_SwitchTo(ClkRCH);
    //系统时钟频率设置与切换
    if(pinlv==RCH4M){
        //RCH 4MHz
        Clk_SwitchTo(ClkRCL);
        Clk_SetRCHFreq(ClkFreq4Mhz);
        Clk_SwitchTo(ClkRCH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCH8M){
        //RCH 8MHz
        Clk_SwitchTo(ClkRCL);
        Clk_SetRCHFreq(ClkFreq8Mhz);
        Clk_SwitchTo(ClkRCH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCH16M){
        //RCH 16MHz
        Clk_SwitchTo(ClkRCL);
        Clk_SetRCHFreq(ClkFreq16Mhz);
        Clk_SwitchTo(ClkRCH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCH22M){
        //RCH 22.12MHz
        Clk_SwitchTo(ClkRCL);
        Clk_SetRCHFreq(ClkFreq22_12Mhz);
        Clk_SwitchTo(ClkRCH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCH24M){
        //RCH 24MHz
        Clk_SwitchTo(ClkRCL);
        Clk_SetRCHFreq(ClkFreq24Mhz);
        Clk_SwitchTo(ClkRCH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==XTH){
        //XTH
        //Clk_SwitchTo(ClkRCL);
        Clk_SwitchTo(ClkXTH);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==XTL){
        //XTL
        Clk_SwitchTo(ClkXTL);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCL38){
        //RCL   38.4K
        Clk_SetRCLFreq(ClkFreq38_4K);
        Clk_SwitchTo(ClkRCL);
        u32Val = Clk_GetHClkFreq();
    }
    else if(pinlv==RCL32){
        //RCL 32768
        Clk_SetRCLFreq(ClkFreq32768);
        u32Val = Clk_GetHClkFreq();
    }

    u32Val = Clk_GetHClkFreq();
    u32Val=0;
}
*/
// led1=pd2 led2=pc6
// key1=pa2 key2=pa1 key3=pc4 key4=pc5 key5=pd4
// mos=pd3
// tx=pd6 rx=pd5

void io_init(u8 _px, u16 pinx, u8 fx)
{
    stc_gpio_init_t stcGpioInit = {0};
    stcGpioInit.u32Pin = pinx;
    if (fx == io_tuiwan)
    {
        stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
        stcGpioInit.u32Pull = GPIO_PULL_NONE;
    }
    if (fx == io_inout)
    {
        stcGpioInit.u32Mode = GPIO_MODE_INPUT;
        stcGpioInit.u32Pull = GPIO_PULL_UP;
    }
    if (fx == io_inlou)
    {
        stcGpioInit.u32Mode = GPIO_MODE_INPUT;
        stcGpioInit.u32Pull = GPIO_PULL_NONE;
    }

    if (_px == _pa)
    {
        SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PA);
        GPIOA_Init(&stcGpioInit);
    }
    else if (_px == _pb)
    {
        SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PB);
        GPIOB_Init(&stcGpioInit);
    }
    else if (_px == _pc)
    {
        SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PC);
        GPIOC_Init(&stcGpioInit);
    }
    else if (_px == _pd)
    {
        SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PD);
        GPIOD_Init(&stcGpioInit);
    }
}

void delay_us(u16 nus)
{
    volatile u8 mm;
    while (nus--)
    {
        mm = 0;
        mm = 1;
        mm = 2;
        mm = 3;
        mm = 4;
        mm = 5;
        mm = 6;
        mm = 7;
        mm = 0;
        mm = 1;
        mm = 2;
        mm = 3;
        mm = 4;
        mm = 5;
        mm = 6;
        mm = 7;
    }
}

void delay_ms(u16 ms)
{
    u16 i, j;
    while (ms--)
    {
        for (i = 40; i != 0; i--)
            for (j = 30; j != 0; j--)
                ;
    }
}

/***************************************************************
看门狗初始化程序
0x00=1.6ms
0x01=3.2ms
0x0a=1.64s
0x0b=3.28s
****************************************************************/
/*
void wdog_init(void)
{
    stc_wdt_config_t  stcWdt_Config;
    DDL_ZERO_STRUCT(stcWdt_Config);
    stcWdt_Config.u8LoadValue = 0x0b;//3.2s
    stcWdt_Config.enResetEnable =WRESET_EN; //WINT_EN;//WRESET_EN;////
    //stcWdt_Config.pfnWdtIrqCb = WdtCallback;   //中断回调函数
    Clk_SetPeripheralGate(ClkPeripheralWdt,TRUE);//
    Wdt_Init(&stcWdt_Config);

    Wdt_Start();
    //Wdt_Feed();//测试喂狗功能时，放开屏蔽
}
*/

/*******************************************************************************
 * ATIM3中断服务函数
 ******************************************************************************/
void ATim3_IRQHandler(void)
{

    // ATIM3 模式0 计时溢出中断
    if (TRUE == ATIM3_GetIntFlag(ATIM3_INT_FLAG_UI))
    {
        MCU_flag.ms_flag++;
        xunSub_timer(10);
        // pwTimer_sub();

        ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI); // ATIM3 模式0 中断标志清除
    }
}

// 时钟初始化
// 注意！！！！如果时钟超过24M,需要设置flash wait为2个周期！！！！！！！！！
void App_Clock_Cfg(void)
{
    stc_sysctrl_sysclk_source_init_t stcSysClkSrc = {0};
    stc_sysctrl_clk_init_t stcSysClk = {0};

    ///< 系统时钟源初始化
    stcSysClkSrc.u32SysClkSourceType = SYSCTRL_SYSCLK_SOURCE_TYPE_RCH; // 时钟源初始化需要操作RCH

    stcSysClkSrc.u32RCHState = SYSCTRL_RCH_TRIM_48MHz |
                               SYSCTRL_RCH_DIV12; // RCH 设置为12分频  -->4M,
    SYSCTRL_SysClkSrcInit(&stcSysClkSrc);         // 时钟源初始化

    ///< 系统时钟初始化
    stcSysClk.u32ClockType = SYSCTRL_CLOCKTYPE_SYSCLK |
                             SYSCTRL_CLOCKTYPE_HCLK |
                             SYSCTRL_CLOCKTYPE_PCLK;       // 时钟初始化需要操作PLL，HCLK和PCLK
    stcSysClk.u32SysClkSource = SYSCTRL_SYSCLK_SOURCE_RCH; // RCH为系统时钟
    stcSysClk.u32HClkDiv = SYSCTRL_SYSCLK_HCLK_PRS1;       // HCLK不分频
    stcSysClk.u32PClkDiv = SYSCTRL_SYSCLK_PCLK_PRS1;       // PCLK不分频
    stcSysClk.u32WaitCycle = SYSCTRL_FLASH_WAIT_CYCLE1;    // 设置flash 读等待为1个周期

    SYSCTRL_SysClkInit(&stcSysClk); // 时钟初始化
}

// ATIM3 配置
void App_Atimer3_Cfg(uint16_t u16Period)
{
    uint16_t u16ArrValue;
    uint16_t u16CntValue;
    stc_atim3_mode0_cfg_t stcAtim3BaseCfg = {0};

    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_ATIM3); // ATIM3 外设时钟使能

    stcAtim3BaseCfg.u32WorkMode = ATIM3_M0_M0CR_WORK_MODE_TIMER;  // 定时器模式
    stcAtim3BaseCfg.u32CountClkSel = ATIM3_M0_M0CR_CT_PCLK;       // 定时器功能，计数时钟为内部PCLK
    stcAtim3BaseCfg.u32PRS = ATIM3_M0_M0CR_ATIM3CLK_PRS16;        // PCLK/16
    stcAtim3BaseCfg.u32CntMode = ATIM3_M0_M0CR_MD_16BIT_ARR;      // 自动重载16位计数器/定时器
    stcAtim3BaseCfg.u32EnTog = ATIM3_M0_M0CR_TOG_OFF;             // TOG关闭
    stcAtim3BaseCfg.u32EnGate = ATIM3_M0_M0CR_GATE_OFF;           // 门控功能关闭
    stcAtim3BaseCfg.u32GatePolar = ATIM3_M0_M0CR_GATE_POLAR_HIGH; //

    ATIM3_Mode0_Init(&stcAtim3BaseCfg); // ATIM3 的模式0功能初始化

    u16ArrValue = 0x10000 - u16Period;
    ATIM3_Mode0_ARRSet(u16ArrValue); // 设置重载值(ARR = 0x10000 - 周期)

    u16CntValue = 0x10000 - u16Period;
    ATIM3_Mode0_Cnt16Set(u16CntValue); // 设置计数初值

    ATIM3_ClearIrqFlag(ATIM3_INT_CLR_UI);    // 清中断标志
    ATIM3_Mode0_EnableIrq();                 // 使能ATIM3中断(模式0时只有一个中断)
    EnableNvic(ATIM3_IRQn, IrqLevel3, TRUE); // ATIM3 开中断
}

void ATim3_init(void)
{
    App_Clock_Cfg();

    App_Atimer3_Cfg(25); // ATIM3 配置; 16分频,周期25000-->25000*(1/4M) * 16 = 100000us = 100ms

    ATIM3_Mode0_Run(); // ATIM3 运行。
}
//***********************************flash read write*****************************
void flash_read(u32 addrs, u8 *buf, u16 len)
{
    u16 i;
    u8 *pu8DataAddr = (u8 *)addrs;
    for (i = 0; i < len; i++)
        buf[i] = pu8DataAddr[i];
}
void flash_writ(u32 addrs, u8 *buf, u16 len)
{
    while (Ok != FLASH_SectorErase(addrs))
    {
        ;
    }
    ///< FLASH 字节写
    while (Ok != FLASH_WriteByte(addrs, buf, len))
    {
        ;
    }
}
void flash_clr(u32 addrs)
{
    while (Ok != FLASH_SectorErase(addrs))
    {
        ;
    }
}

//***************************************串口1***************************************

/// 串口1

///< LPUART配置
void App_LpUartCfg(void)
{
    stc_lpuart_init_t stcInit = {0};

    ///< 外设模块时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_LPUART1);

    ///< LPUART 初始化
    stcInit.u32StopBits = LPUART_STOPBITS_1;             ///< 1停止位
    stcInit.u32Parity = LPUART_PARITY_NONE;              ///< 无校验
    stcInit.u32FrameLength = LPUART_FRAME_LEN_8BITS;     ///< 帧长8位
    stcInit.u32TransMode = LPUART_MODE_TX_RX;            ///< 发送模式
    stcInit.stcBaudRate.u32SclkSrc = LPUART_CLKSRC_PCLK; ///< 传输时钟源
    stcInit.stcBaudRate.u32Sclk = SYSCTRL_GetPCLK();     ///< PCLK获取
    stcInit.stcBaudRate.u32BaudRate = 9600;              ///< 波特率

    LPUART_Init(LPUART1, &stcInit);
}

///< 端口配置
void App_LpUartPortCfg(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    ///< 外设时钟使能
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_PD);

    GPIO_PD05_SET();
    stcGpioInit.u32Mode = GPIO_MODE_OUTPUT_PP;
    stcGpioInit.u32Pin = GPIO_PIN_05;
    stcGpioInit.u32Pull = GPIO_PULL_UP;
    GPIOD_Init(&stcGpioInit);

    stcGpioInit.u32Mode = GPIO_MODE_INPUT;
    stcGpioInit.u32Pin = GPIO_PIN_06;
    GPIOD_Init(&stcGpioInit);

    GPIO_PD05_AF_LPUART1_TXD_SET();
    GPIO_PD06_AF_LPUART1_RXD_SET();
}

void LPUART1_init(void)
{

    ///< 端口配置
    App_LpUartPortCfg();

    ///< LPUART配置
    App_LpUartCfg();

    ///< 起始发送
    // LPUART_Transmit(LPUART1, pu8TestData, 2, 0xFFFFFF);

    LPUART_ClearIrq(LPUART1, LPUART_FLAG_ALL);
    LPUART_EnableIrq(LPUART1, LPUART_INT_RC);

    EnableNvic(LPUART1_IRQn, IrqLevel2, TRUE);
    CH_L;
}

#define StackLen_Value 100
u8 BufRead_JiShi = 0; /////收包时间计数器

//////接收数据处理
void UART1_ReadBoxSUB(void)
{

    BufRead_JiShi++;
    if (BufRead_JiShi >= 5)
    {
        if (MCU_flag.Rbuf_num > 0)
        { // 收到数据处理

            GetDataWork(MCU_flag.Rbuf);
            // bg595_ceshi(MCU_flag.Rbuf);
        }
        MCU_flag.Rbuf_num = 0;
    }
}

////////////////////////////////////////////////////buf-delay//////////////////////////////
////数据发送
void UART1_SendBox(u8 *buf, u8 len)
{
    CH_H;
    LPUART_Transmit(LPUART1, buf, len, 0xFFFFFF);
    delay_us(300);
    CH_L;
}

//////接收中断处理
/////数据长度，堆栈长度
void UART1_R_Interr(u8 datas, u8 StackLen)
{
    if (StackLen == 0)
        return;
    MCU_flag.Rbuf[MCU_flag.Rbuf_num] = datas;
    if (MCU_flag.Rbuf_num < (StackLen - 1))
        MCU_flag.Rbuf_num++;
    else
        MCU_flag.Rbuf_num = 0;
    BufRead_JiShi = 0;
}

u8 rxbuf[1] = {0};
void LpUart1_IRQHandler(void)
{
    if (LPUART_GetFlag(LPUART1, LPUART_FLAG_RC))
    { // 接收完成
        if (LPUART_GetFlag(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE))
        {
            ///< 错误处理……
            LPUART_ClearIrq(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE);
        }
        else
        {
            LPUART_ReceiveIT(LPUART1, rxbuf);
            LPUART_ClearIrq(LPUART1, LPUART_FLAG_RC);
            UART1_R_Interr(rxbuf[0], StackLen_Value);
            // LPUART_EnableIrq(LPUART1, LPUART_INT_TXE);
            // LPUART_DisableIrq(LPUART1, LPUART_INT_RC);
        }
    }

    if (LPUART_GetFlag(LPUART1, LPUART_FLAG_TXE))
    { // 发送缓存为空
        LPUART_ClearIrq(LPUART1, LPUART_FLAG_TXE);
    }
}

////////////
u8 mm[2];
void UART1_try(void)
{
    // Uart_SendDataIt(M0P_UART1, 55);//发送数据
    // Uart_SetTb8(M0P_UART1,UartEven,0);
    // Uart_SendDataIt(M0P_UART1,MCU_flag.Debug8_reg++);
    // Uart_SetTb8(M0P_UART1,UartEven,2);
    // Uart_SendDataPoll(M0P_UART1,3);
    mm[0]++;
    UART1_SendBox(mm, 2);
}

stc_wwdt_init_t gstcWwdtInit = {0};

void App_WdtInit(void)
{
    ///< 开启WWDT外设时钟
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_WWDT);

    ///< WWDT 初始化
    ///< 溢出时间：T_wwdt= ((PCLK分频数 * (CR0.WCNT - 0x3F))/PCLK频率)
    ///< 本例为：(524288 *8/4000000) ≈ 1s
    gstcWwdtInit.u32PreOverInt = WWDT_PRE_INT_ENABLE;
    gstcWwdtInit.u32Counter = 0x3F + 8;
    gstcWwdtInit.u32Window = 0x7F;
    gstcWwdtInit.u32Prescaler = WWDT_PCLK_DIV_524288;
    WWDT_Init(&gstcWwdtInit);
    ///< 启动 WDT
    WWDT_Start();
    WWDT_Feed(gstcWwdtInit.u32Counter);
}

void wwFeed(void)
{
    WWDT_Feed(gstcWwdtInit.u32Counter);
}

void Data_save20(u8 *data)
{
    flash_writ(faddr1, data, 20);
}
void Data_load20(u8 *dt)
{
    flash_read(faddr1, dt, 20);
}

///< ADC模块 初始化
void Adc_init(void)
{
    stc_adc_cfg_t stcAdcCfg = {0};

    /* 第一步：开启ADC/BGR 外设时钟 */
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_ADC);

    /* 第三步：ADC 初始化配置 */
    stcAdcCfg.u32ClkDiv = ADC_CLOCK_PCLK_DIV128;          ///< 采样分频-128
    stcAdcCfg.u32SampCycleSel = ADC_SAMPLINGTIME_12CYCLE; ///< 采样周期数-12
    stcAdcCfg.u32RefVolSel = ADC_REF_VOLTAGE_AVCC;        ///< 参考电压选择-AVCC
    stcAdcCfg.u32InputSource = ADC_EXINPUT_PA02;          ///< 输入信号选择-PA01
    // stcAdcCfg.u32ExtTriggerSource = ADC_EXTTRIGGER_PB03;       ///< 外部触发信号来自-PB03
    ADC_Init(ADC, &stcAdcCfg);

    /* 第四步：清除中断标志位 */
    ADC_ClearFlag_EOC(ADC);
}

u32 get_adc(void)
{
    ///< 开启单次转换
    ADC_StartSingleConversion(ADC); ///< 产生EOC标志
    while (!ADC_IsActiveFlag_EOC(ADC))
        ; // 等待转换结束

    ADC_ClearFlag_EOC(ADC);

    return ADC_GetResult(ADC); ///< 获取采样值
}
