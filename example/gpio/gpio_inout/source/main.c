#include "led.h"
#include "sys.h"
#include "key.h"
#include "xunSub.h"

/************************************
**弱电A13代灯控-背光转换，全部用595
**芯片端口定义
//led背光
//led1=pd03(20) led2=pa03(10) led3=pb04(12)
//led4=pd04(1) led5=pc03(13) led6=pb05(11)
//led7=pa01(5)
//按键AD转换
//key=ain8(pa02)(6)
//RS485
//tx=pd05(2) rx=pd06(3) ch=pc04(14)
//调位
//s1=pc06(16) s2=pc05(15) s3=pa04(8)
//手感应
//shou=pd02(19)
************************************/
extern Pro_flag_s MCU_flag;
extern u8 h595_buf[2];

void initRCU(void)
{
	MCU_flag.ver = 0x20;
	// 0x20=定制横向两键，对应按键版本V1.0
	// led

	io_init(_pc, pin3, io_tuiwan);
	io_init(_pb, pin5, io_tuiwan);
	io_init(_pa, pin1, io_tuiwan);

	// key1-key6 adc8
	Adc_init();
	// s1 s2 s3
	io_init(_pc, pin6, io_inout);
	io_init(_pc, pin5, io_inout);
	io_init(_pa, pin4, io_inout);
	// shou
	io_init(_pd, pin2, io_inout);
	// ch485
	io_init(_pc, pin4, io_tuiwan);

	ATim3_init();
	LPUART1_init();

	/*手感应使能位*/
	MCU_flag.shou_en = 0;

	dk485_init();
	MCU_flag.yanchi = 1; // 15s   常规8X5=40s
	MCU_flag.lv = 3;
	bg_en();
	MCU_flag.MyMAC = Get_set();

	// MCU_flag.Debug32_reg=get_adc();
	App_WdtInit();
}
void Thread_Ms_sub(void)
{
	if (MCU_flag.ms_flag >= 10)
	{
		xunSub_diao();
		MCU_flag.ms_flag = 0;
	}
}

int main(void)
{
	initRCU();
	while (1)
	{
		Thread_Ms_sub();
		wwFeed();
	}
}
