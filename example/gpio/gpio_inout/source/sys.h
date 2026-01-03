#ifndef __SYS_H
#define __SYS_H
#include "gpio.h"
// #include "bt.h"
// #include "uart.h"
#include "flash.h"
#include "RelayAPP.h"
#include "atim3.h"
#include "lpuart.h"
#include "wwdt.h"
#include "adc.h"

#include <stdio.h>

#define uchar unsigned char
#define uint unsigned int
#define uint32 unsigned long
#define ulong unsigned long

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long

#define p0 0
#define p1 1
#define p2 2
#define p3 3

#define _pa 0
#define _pb 1
#define _pc 2
#define _pd 3

#define pin0 GPIO_PIN_00
#define pin1 GPIO_PIN_01
#define pin2 GPIO_PIN_02
#define pin3 GPIO_PIN_03
#define pin4 GPIO_PIN_04
#define pin5 GPIO_PIN_05
#define pin6 GPIO_PIN_06
#define pin7 GPIO_PIN_07

#define pin8 GPIO_PIN_8
#define pin9 GPIO_PIN_9
#define pin10 GPIO_PIN_10
#define pin11 GPIO_PIN_11
#define pin12 GPIO_PIN_12
#define pin13 GPIO_PIN_13
#define pin14 GPIO_PIN_14
#define pin15 GPIO_PIN_15

#define RCH4M 0
#define RCH8M 1
#define RCH16M 2
#define RCH22M 3
#define RCH24M 4
#define XTH 5
#define XTL 6
#define RCL38 7
#define RCL32 8

#define bit0 0x01
#define bit1 0x02
#define bit2 0x04
#define bit3 0x08
#define bit4 0x10
#define bit5 0x20
#define bit6 0x40
#define bit7 0x80
#define io_inout 0  // 上拉输入-输出
#define io_tuiwan 1 // 推挽输出
#define io_in 2     // 输入
#define io_inlou 3  // 开漏输入
// 存储地址，16K的4000Hbyte   占用最后256个字节
#define flash_addrS 0x3F00

#define StaLed_Mie 0x00
#define StaLed_Liang 0x01
#define StaLed_Man 0x02
#define StaLed_kuai 0x03
#define StaLed_Lian3 0x04
// 充电状态
#define cdSta_nc 0x00  // 闲置状态
#define cdSta_shu 0x01 // 输入状态
#define cdSta_cd 0x02  // 充电状态

// 0x4000-0x41ff
// 0x4200-0x43ff
// 0x4400-0x45ff
// 0x4600-0x47ff
// flash地址1-4
#define faddr1 0x4000
#define faddr2 0x4200
#define faddr3 0x4400
#define faddr4 0x4600
// pc04
#define CH_H GPIO_PC04_SET()
#define CH_L GPIO_PC04_RESET()

//////////////////////////////////////////////////
typedef struct Pro_flag
{
    u8 ver;
    u8 key_s;
    u8 sysMode;
    u8 ms_flag;
    u16 ms100_flag;
    // u8 Debug8_reg;
    // u16 Debug16_reg;
    // u16 Debug32_reg;
    // u8 Debug_buf[30];
    u8 MyMAC;
    u8 keyaddr[6]; // 按键地址，对应的IO地址
    u8 yanchi;     // 背光延迟，多久熄灭
    u8 lv;         // 背光亮度等级
    // 橙色背光亮灭
    u8 bgen0to7;
    u8 bgen8to15;
    u8 bgen16to23;
    u8 bgen24to32;
    // 背光64路
    // u8 bg[18];
    u8 Rbuf_num;
    u8 Rbuf[100];

    // 协议选择
    u8 xy_sle; // 1=一体机 2=弱电魔盒
    // 魔盒协议
    u8 mb_addr;   // 0-15
    u8 mh_bg[32]; // 每个面板地址6路背光
    u8 qz_bg[16]; // 强制背光---面板 地址0到地址8 ，当bit0置1时，背光强制关闭，置0后可以灵活开关
    u8 bg_yanchi;
    // 手感应触发使能
    u8 shou_en;
    u8 shou_stop;
    // 595背光状态缓存-转换的没有第七路
    u8 led[7];  // 6路背光0=关白灯亮黄灯1=反向（字节7）=弱背光比例
    u8 guanLed; // 关闭led，每一bit关闭一路led
} Pro_flag_s;

void App_ClkInit(u8 pinlv);

void io_init(u8 _px, u16 pinx, u8 fx);
u16 Get_UID(u8 *buf);

void delay_us(u16 nus);
void delay_ms(u16 ms);
unsigned int crc_chk_value(unsigned char *data_value, u16 length);
u8 GetCheckCodeX(u8 *cmd, u8 BoxLen);
u8 GetCheckHe(u8 *cmd, u8 BoxLen);
u16 BCC_CheckSum(u8 *buf, u8 len);
uint8_t calcrc_data(uint8_t *rxbuf, uint8_t len);

void wdog_init(void);

void ATim3_init(void);

void flash_read(u32 addrs, u8 *buf, u16 len);
void flash_writ(u32 addrs, u8 *buf, u16 len);
void flash_clr(u32 addrs);

void App_WdtInit(void);
void wwFeed(void);

void LPUART1_init(void);
void UART1_ReadBoxSUB(void);
void UART1_SendBox(u8 *buf, u8 len);

void UART1_try(void);
void Data_save20(u8 *data);
void Data_load20(u8 *data);

void Adc_init(void);
u32 get_adc(void);

#endif
