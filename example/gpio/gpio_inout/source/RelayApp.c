#include "RelayAPP.h"
#include "sys.h"

#include "xunSub.h"

#if (dingzhi_sle == 0)

extern u16 xybg_num;

extern Pro_flag_s MCU_flag;

u8 Shan_flag = 0;
void Get_Mac(void)
{
	u8 cd[30] = {0};
	Data_load20(cd);
	if ((cd[8] * 256 + cd[9]) == BCC_CheckSum(cd, 8))
	{
		MCU_flag.keyaddr[0] = cd[2];
		MCU_flag.keyaddr[1] = cd[3];
		MCU_flag.keyaddr[2] = cd[4];
		MCU_flag.keyaddr[3] = cd[5];
		MCU_flag.keyaddr[4] = cd[6];
		MCU_flag.keyaddr[5] = cd[7];
	}
	else
	{
		MCU_flag.keyaddr[0] = 1;
		MCU_flag.keyaddr[1] = 2;
		MCU_flag.keyaddr[2] = 3;
		MCU_flag.keyaddr[3] = 4;
		MCU_flag.keyaddr[4] = 5;
		MCU_flag.keyaddr[5] = 6;
	}

	// 如果是魔盒协议
	if ((cd[0] >= 0xc0) && (cd[0] < 0xe0) && (cd[1] == 0x0e) && (cd[2] == 0x0e) && (cd[3] == 0x0e) && (cd[4] == 0x0e))
	{
		MCU_flag.mb_addr = cd[0] - 0xc0;
	}
	else
	{
		MCU_flag.mb_addr = 0;
	}
}

void dk485_init(void)
{
	Get_Mac();
	MCU_flag.xy_sle = 2;
	// MCU_flag.mb_addr=0;
}
/*请求设置地址码     0xb1 0xb2 0xb3*/
u8 qingqiu_set = 0;
u8 qingqiu_num = 0;
void qingqiu_send(void)
{
	u8 bt[3];
	bt[0] = 0xb1;
	bt[1] = 0xb2;
	bt[2] = 0xb3;
	UART1_SendBox(bt, 3);
	qingqiu_set = 1;
	qingqiu_num = 20;
}
// 0.1s
void qingqiu_sub(void)
{
	if (qingqiu_num > 0)
	{
		qingqiu_num--;
		if (qingqiu_num == 0)
		{
			qingqiu_set = 0;
		}
	}
}
u8 bg_flag = 0; // 0=低亮 1=高亮 2=延时

/*数据处理*/
void GetDataWork(u8 *Buf)
{
	u16 s1 = 0;
	if (Buf[0] == sb_head)
	{
		/*
		if(Buf[1]==sb_bgBit)
		{//背光处理
			if((Buf[12]*256+Buf[13])!=BCC_CheckSum(Buf,12))
			{
				return;
			}
			MCU_flag.xy_sle=1;
			MCU_flag.bg[0]=Buf[2];
			MCU_flag.bg[1]=Buf[3];
			MCU_flag.bg[2]=Buf[4];
			MCU_flag.bg[3]=Buf[5];
			MCU_flag.bg[4]=Buf[6];
			MCU_flag.bg[5]=Buf[7];
			MCU_flag.bg[6]=Buf[8];
			MCU_flag.bg[7]=Buf[9];
			MCU_flag.lv=Buf[10];
			MCU_flag.yanchi=Buf[11];
		}
		else if(Buf[1]==sb_SetDk)
		{//设置灯控地址
			if((Buf[8]*256+Buf[9])!=BCC_CheckSum(Buf,8))
			{
				return;
			}

			MCU_flag.keyaddr[0]=Buf[2];
			MCU_flag.keyaddr[1]=Buf[3];
			MCU_flag.keyaddr[2]=Buf[4];
			MCU_flag.keyaddr[3]=Buf[5];
			MCU_flag.keyaddr[4]=Buf[6];
			MCU_flag.keyaddr[5]=Buf[7];

			Data_save20(Buf);
			Get_Mac();
			//背光闪烁
			BgShan_en();
		}
		*/
	}
	else if ((Buf[0] == 0xff) && (Buf[1] == 0xaa))
	{ // 魔盒--背光反码
		if (dingzhi_yanshi != 1)
		{ // 按键背景灯一亮一灭，做演示用的时候不要校验
			if (Buf[20] != calcrc_data(Buf, 20))
			{
				if ((Buf[21] == 0xf1) && (Buf[22] == 0xf2))
					Buf[21] = 0xf1; /*测试使用*/
				else
					return;
			}
		}
		// bg_en();
		MCU_flag.xy_sle = 2;
		MCU_flag.lv = 3;			// Buf[2];
		MCU_flag.yanchi = 3;		// Buf[3];
		MCU_flag.bgen0to7 = Buf[2]; // Byte2 Byte 3 :表示按键动作后，面板的上面按键全关掉的背光指示
		// 位值：1 =表示灭掉背光（按键动作后灭掉），0 =表示不灭背光（按键动作后变半亮）
		MCU_flag.bgen8to15 = Buf[3]; // Byte 2 表示面板地址 0~7  的背光 Byte 3 表示面板地址 8~16 的背光
		MCU_flag.mh_bg[0] = Buf[4];	 // 地址0
		MCU_flag.mh_bg[1] = Buf[5];	 // 地址1
		MCU_flag.mh_bg[2] = Buf[6];	 // 地址2
		MCU_flag.mh_bg[3] = Buf[7];	 // 地址3
		MCU_flag.mh_bg[4] = Buf[8];	 // 地址4
		MCU_flag.mh_bg[5] = Buf[9];	 // 地址5
		MCU_flag.mh_bg[6] = Buf[10]; // 地址6
		MCU_flag.mh_bg[7] = Buf[11]; // 地址7
		MCU_flag.mh_bg[8] = Buf[12]; // 地址8
		MCU_flag.mh_bg[9] = Buf[13];
		MCU_flag.mh_bg[10] = Buf[14];
		MCU_flag.mh_bg[11] = Buf[15];
		MCU_flag.mh_bg[12] = Buf[16];
		MCU_flag.mh_bg[13] = Buf[17];
		MCU_flag.mh_bg[14] = Buf[18];
		MCU_flag.mh_bg[15] = Buf[19];  // 地址15
		MCU_flag.bgen16to23 = Buf[23]; // Byte2 Byte 3 :表示按键动作后，面板的上面按键全关掉的背光指示
		// 位值：1 =表示灭掉背光（按键动作后灭掉），0 =表示不灭背光（按键动作后变半亮）
		MCU_flag.bgen24to32 = Buf[24]; // Byte 2 表示面板地址 0~7  的背光 Byte 3 表示面板地址 8~16 的背光
		MCU_flag.mh_bg[16] = Buf[27];  // 地址16
		MCU_flag.mh_bg[17] = Buf[28];
		MCU_flag.mh_bg[18] = Buf[29];
		MCU_flag.mh_bg[19] = Buf[30];
		MCU_flag.mh_bg[20] = Buf[31];
		MCU_flag.mh_bg[21] = Buf[32];
		MCU_flag.mh_bg[22] = Buf[33]; // 地址22
		MCU_flag.mh_bg[23] = Buf[34];
		MCU_flag.mh_bg[24] = Buf[35];

		MCU_flag.mh_bg[25] = Buf[36];
		MCU_flag.mh_bg[26] = Buf[37];
		MCU_flag.mh_bg[27] = Buf[38];
		MCU_flag.mh_bg[28] = Buf[39];
		MCU_flag.mh_bg[29] = Buf[40];
		MCU_flag.mh_bg[30] = Buf[41];
		MCU_flag.mh_bg[31] = Buf[42];
		MCU_flag.qz_bg[0] = Buf[43];  // 地址0
		MCU_flag.qz_bg[1] = Buf[44];  // 地址1
		MCU_flag.qz_bg[2] = Buf[45];  // 地址2
		MCU_flag.qz_bg[3] = Buf[46];  // 地址3
		MCU_flag.qz_bg[4] = Buf[47];  // 地址4
		MCU_flag.qz_bg[5] = Buf[48];  // 地址5
		MCU_flag.qz_bg[6] = Buf[49];  // 地址6
		MCU_flag.qz_bg[7] = Buf[50];  // 地址7
		MCU_flag.qz_bg[8] = Buf[51];  // 地址8
		MCU_flag.qz_bg[9] = Buf[52];  // 地址9
		MCU_flag.qz_bg[10] = Buf[53]; // 地址10
		MCU_flag.qz_bg[11] = Buf[54]; // 地址11
		MCU_flag.qz_bg[12] = Buf[55]; // 地址12
		MCU_flag.qz_bg[13] = Buf[56]; // 地址13
		MCU_flag.qz_bg[14] = Buf[57]; // 地址14
		MCU_flag.qz_bg[15] = Buf[58]; // 地址15
		MCU_flag.bg_yanchi = Buf[26]; // 背光延迟
		// 背光延时时间  	0默认5S		超过200	不延时
		xybg_led();
		if (MCU_flag.mb_addr <= 15) // 完全关闭背光
			MCU_flag.guanLed = MCU_flag.qz_bg[MCU_flag.mb_addr];
	}
	else if ((Buf[1] == 0x0e) && (Buf[2] == 0x0e))
	{ // 魔盒--设置
		if (MCU_flag.xy_sle != 2)
			return;
		if ((Buf[0] >= 0xc0) && (Buf[0] < 0xe0) && (Buf[1] == 0x0e) && (Buf[2] == 0x0e) && (Buf[3] == 0x0e) && (Buf[4] == 0x0e))
		{
			MCU_flag.mb_addr = Buf[0] - 0xc0;
			Data_save20(Buf);
			// 背光闪烁
			BgShan_en();
		}
	}
	else if ((Buf[0] == 0x1b) && (Buf[1] == 0x2c) && (Buf[3] == 0x3d))
	{ // 魔盒--设置，请求自动获取地址
		if (MCU_flag.xy_sle != 2)
			return;
		if (qingqiu_set == 0)
			return;
		if (Buf[2] < 32)
		{
			MCU_flag.mb_addr = Buf[2];
			Buf[0] = MCU_flag.mb_addr + 0xc0;
			Buf[1] = 0x0e;
			Buf[2] = 0x0e;
			Buf[3] = 0x0e;
			Buf[4] = 0x0e;
			Data_save20(Buf);
			// 背光闪烁
			BgShan_en();
			qingqiu_set = 0;
		}
	}
	else if ((Buf[0] == 0xca) && (Buf[1] == 0xb6) && (Buf[4] == 0xbb))
	{ // 手感应发485码，开背光
		/*发送手感应亮背光码     0xca 0xb6 0xff 0xff 0xbb*/

		s1 = (Buf[2] << 8) + Buf[3];
		s1 = (1 << MCU_flag.mb_addr) & s1;
		if (s1 > 0)
			bg_en();
	}
}

/*背光一亮一灭,用来演示,地址必须是0或者1*/
////展会演示
u8 rr[6] = {0, 0, 0, 0, 0, 0};
u8 ky = 0;
void ruo_yanshi(u8 keys)
{
	u8 buf[50];
	u8 cc[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	if (dingzhi_yanshi != 1)
		return;
	buf[0] = 0xff;
	buf[1] = 0xaa;
	buf[2] = 0;
	buf[3] = 0;
	if (rr[keys - 1] > 0)
	{
		rr[keys - 1] = 0;
		ky = ky & (~cc[keys - 1]);
	}
	else
	{
		rr[keys - 1] = 1;
		ky = ky | cc[keys - 1];
	}
	buf[4] = ky;
	buf[5] = ky;
	buf[6] = ky;
	buf[7] = ky;
	buf[8] = ky;
	buf[9] = ky;
	buf[10] = ky;
	buf[11] = ky;
	buf[12] = ky;
	buf[13] = 0;
	buf[14] = 0;
	buf[15] = 0;
	buf[16] = 0;
	buf[17] = 0;
	buf[18] = 0;
	buf[19] = 0;
	buf[20] = 0;
	buf[21] = 0;
	buf[22] = 0;
	buf[23] = 0;
	buf[24] = 0;
	buf[25] = 0;
	buf[26] = 0;
	buf[27] = 0;
	buf[28] = 0;
	buf[29] = 0;
	buf[30] = 0;
	buf[31] = 0;
	buf[32] = 0;
	buf[33] = 0;
	buf[34] = 0;
	buf[35] = 0;
	buf[36] = 0;
	buf[37] = 0;
	buf[38] = 0;
	buf[39] = 0;
	buf[40] = 0;
	buf[41] = 0;
	buf[42] = 0;
	buf[43] = 0;
	buf[44] = 0;
	buf[45] = 0;
	buf[46] = 0;
	buf[47] = 0;
	buf[48] = 0;
	buf[49] = 0;
	GetDataWork(buf);
}
/*按键发码*/
void key_send(u8 keys, u8 dw)
{
	u8 bt[10];
	u16 crc;
	bt[0] = sb_head;
	bt[1] = sb_keyaddr;
	bt[2] = MCU_flag.keyaddr[keys - 1];
	bt[3] = dw;
	crc = BCC_CheckSum(bt, 4);
	bt[4] = crc / 256;
	bt[5] = crc % 256;
	UART1_SendBox(bt, 6);
}

/*背光处理函数*/
void BgBit_sub(void)
{
	// u8 t1,t2;
	u8 key_tp = MCU_flag.key_s; /*键位数量，根据跳位来做*/

	if (Shan_flag == 1)
		return;
	if (MCU_flag.xy_sle == 2)
	{ // 魔盒背光

		if ((key_tp == 6) | (key_tp == 4))
		{
			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x01)
				ledB_set(1, 100);
			else
				ledB_set(1, 0);

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x02)
				ledB_set(2, 100);
			else
				ledB_set(2, 0);

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x04)
				ledB_set(3, 100);
			else
				ledB_set(3, 0);

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x08)
				ledB_set(4, 100);
			else
				ledB_set(4, 0);

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x10)
				ledB_set(5, 100);
			else
				ledB_set(5, 0);

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x20)
				ledB_set(6, 100);
			else
				ledB_set(6, 0);
		}
		else if (key_tp == 3)
		{ // 三键横向---左侧三个led不亮
			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x01)
			{
				ledB_set(1, 100);
				ledB_set(2, 100);
			}
			else
			{
				ledB_set(1, 0);
				ledB_set(2, 0);
			}

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x02)
			{
				ledB_set(5, 100);
				ledB_set(6, 100);
			}
			else
			{
				ledB_set(5, 0);
				ledB_set(6, 0);
			}

			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x04)
			{
				ledB_set(3, 100);
				ledB_set(4, 100);
			}
			else
			{
				ledB_set(3, 0);
				ledB_set(4, 0);
			}
		}
		else if (key_tp == 1)
		{
			if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x01)
			{
				ledB_set(1, 0);
				ledB_set(2, 0);
				ledB_set(3, 0);
				ledB_set(4, 0);
				ledB_set(5, 100);
				ledB_set(6, 100);
			}
			else
			{
				ledB_set(1, 0);
				ledB_set(2, 0);
				ledB_set(3, 0);
				ledB_set(4, 0);
				ledB_set(5, 0);
				ledB_set(6, 0);
			}
		}
		else if (key_tp == 2)
		{
			if (MCU_flag.ver == 0x20)
			{ // 定制
				if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x01)
				{
					ledB_set(1, 100);
					ledB_set(4, 100);
				}
				else
				{
					ledB_set(1, 0);
					ledB_set(4, 0);
				}

				if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x02)
				{
					ledB_set(3, 100);
					ledB_set(2, 100);
				}
				else
				{
					ledB_set(3, 0);
					ledB_set(2, 0);
				}
			}
			else
			{
				if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x01)
				{
					ledB_set(1, 100);
					ledB_set(2, 100);
				}
				else
				{
					ledB_set(1, 0);
					ledB_set(2, 0);
				}

				if (MCU_flag.mh_bg[MCU_flag.mb_addr] & 0x02)
				{
					ledB_set(3, 100);
					ledB_set(4, 100);
				}
				else
				{
					ledB_set(3, 0);
					ledB_set(4, 0);
				}
			}
		}
	}
}

/*背光闪烁*/
u16 shan_num = 0;
void BgShan_sub(void)
{
	shan_num++;
	if (Shan_flag == 1)
	{
		if (shan_num < 50)
		{
			led_set(1, 100);
			led_set(2, 100);
			led_set(3, 100);
			led_set(4, 100);
			led_set(5, 100);
			led_set(6, 100);
			led_set(7, 100);
			led_set(8, 100);
			led_set(9, 100);
			led_set(10, 100);
			led_set(11, 100);
			led_set(12, 100);
		}
		else if ((shan_num >= 50) & (shan_num <= 100))
		{
			led_set(1, 0);
			led_set(2, 0);
			led_set(3, 0);
			led_set(4, 0);
			led_set(5, 0);
			led_set(6, 0);
			led_set(7, 0);
			led_set(8, 0);
			led_set(9, 0);
			led_set(10, 0);
			led_set(11, 0);
			led_set(12, 0);
		}
		else
		{

			Shan_flag = 0;
		}
	}
}

void BgShan_en(void)
{
	Shan_flag = 1;
	shan_num = 0;
}

/*键位选择转换*/
u8 key_sle(u8 keys)
{
	u8 key_tp = MCU_flag.key_s;

	/*6键跳位*/
	if (key_tp == 6)
	{
		return keys;
	}
	else if (key_tp == 3)
	{
		if ((keys == 1) | (keys == 2))
			return 1;
		else if ((keys == 5) | (keys == 6))
			return 2;
		else if ((keys == 3) | (keys == 4))
			return 3;
	}
	else if (key_tp == 1)
	{
		return 1;
	}
	/*4键跳位*/

	else if (key_tp == 4)
	{
		return keys;
	}
	else if (key_tp == 2)
	{
		if (MCU_flag.ver == 0x20)
		{ // 定制
			if ((keys == 1) | (keys == 4))
				return 1;
			else if ((keys == 3) | (keys == 2))
				return 2;
		}
		else
		{
			if ((keys == 1) | (keys == 2))
				return 1;
			else if ((keys == 3) | (keys == 4))
				return 2;
		}
	}
	return keys;
}

/*按键发码--林*/
void key_sendLin(u8 keys, u8 dw)
{
	u8 bt[10];
	u8 keyxx = 0;
	keyxx = key_sle(keys);
	bt[0] = 0xaa;
	bt[1] = dw; // 0 = 按下,1=松开,目前松开不需要发码；
	bt[2] = 0;
	bt[3] = MCU_flag.mb_addr; // MCU_flag.keyaddr[keys-1];
	bt[4] = keyxx;			  // key_num
	bt[5] = calcrc_data(bt, 5);
	UART1_SendBox(bt, 6);
}
/*获取字节中的bit位重新组合*/
/*例如：0b0011 1110取第1位开始的5个bit，则获得0b0001 1111*/

u8 Get_bit(u8 dat, u8 st, u8 num)
{
	return (dat >> st) & (2 ^ num);
}

/*发送手感应亮背光码     0xca 0xb6 0xff 0xff 0xbb*/
void shou_send(void)
{
	u8 bt[10];
	bt[0] = 0xca;
	bt[1] = 0xb6; // 0 = 按下,1=松开,目前松开不需要发码；
	bt[2] = 0xff;
	bt[3] = 0xff;
	bt[4] = 0xbb; // key_num

	UART1_SendBox(bt, 5);
}

#endif
