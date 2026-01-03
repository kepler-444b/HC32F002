#include "key.h"
#include "led.h"
#include "RelayAPP.h"

#include "usart.h"

extern Pro_flag_s MCU_flag;
void key_init(void)
{
}

// wu=1023
// 1=0
// 2=109
// 3=287
// 4=473
// 5=631
// 6=755
#define key_cha 60
#define key1 0
#define key2 109
#define key3 287
#define key4 473
#define key5 631
#define key6 755
u8 key_get(void)
{

	u8 temp = 0;
	u32 abc = get_adc();

	if (abc <= (key1 + key_cha))
		temp = 0x01;
	else if ((abc <= (key2 + key_cha)) & (abc >= (key2 - key_cha)))
		temp = 0x02;
	else if ((abc <= (key3 + key_cha)) & (abc >= (key3 - key_cha)))
		temp = 0x04;
	else if ((abc <= (key4 + key_cha)) & (abc >= (key4 - key_cha)))
		temp = 0x08;
	else if ((abc <= (key5 + key_cha)) & (abc >= (key5 - key_cha)))
		temp = 0x10;
	else if ((abc <= (key6 + key_cha)) & (abc >= (key6 - key_cha)))
		temp = 0x20;

	return temp;
}

u8 key_reg;
u16 key_count;
u8 key_reg_out;
u8 GetKey_mode(void)
{
	u8 i; ////,temp_count,j,k;
	u8 key_mode;
	u16 temp;
	u16 key_temp = 0;
	u16 key_temp0 = 0;
	// u16  key_temp1=0;
	//********key get sub**********************
	key_temp0 = (u16)key_get();

	if (key_temp0 > 0)
	{
		key_temp = key_temp0;
	}
	else
	{
		// key_temp=key_temp1;
		key_temp = 0;
	}
	// key_temp=0x04;
	//*****************************************
	if (key_temp > 0)
	{
		if (key_reg == 0)
		{
			for (i = 0; i < 16; i++)
			{
				if (key_temp & (1 << i))
				{
					if (key_temp0 > 0)
						key_reg = i + 1;
					else
						key_reg = i + 17;
					break;
				}
			}
		}
		else
		{
			if (key_temp0 > 0)
				temp = key_temp & (1 << (key_reg - 1));
			else
				temp = key_temp & (1 << (key_reg - 17));
			if (temp > 0)
			{
				key_count++;
				if ((key_count > key_short_down) & (key_long_down > key_count))
				{
					key_mode = 1;
					key_reg_out = key_reg;
				}
				else if (key_count > key_long_down)
				{
					key_mode = 2;
					key_reg_out = key_reg;
				}
			}
			else
			{
				if ((key_count > key_short_down) & (key_long_down > key_count))
				{
					key_mode = 3;
					key_reg_out = key_reg;
				}
				else if (key_count > key_long_down)
				{
					key_mode = 4;
					key_reg_out = key_reg;
				}
				key_count = 0;
				key_reg = 0;
			}
		}
	}
	else
	{
		if (key_reg > 0)
		{
			if ((key_count > key_short_down) & (key_long_down > key_count))
			{
				key_mode = 3;
				key_reg_out = key_reg;
			}
			else if (key_count > key_long_down)
			{
				key_mode = 4;
				key_reg_out = key_reg;
			}
			key_count = 0;
			key_reg = 0;
		}
	}
	return key_mode;
}

u8 opare_flag;
void key_work(uchar key_g, uchar key_mode_g)
{

	if (key_mode_g == 1)
	{
		if ((opare_flag & 0x01) == 0)
		{
			// short down
			//-----
			// bg_en();
			// ruo_yanshi(key_g);//Õ¹»áÑÝÊ¾
			key_sendLin(key_g, key_Down);

			// led_set(key_g,1);
			//-----
			opare_flag |= 0x01;
		}
	}
	else if (key_mode_g == 2)
	{
		if ((opare_flag & 0x10) == 0)
		{
			// long down
			//-----
			qingqiu_send();
			delay1ms(100);
			key_sendLin(key_g, key_long_d);
			//-----
			opare_flag |= 0x10;
		}
	}
	else if (key_mode_g == 3)
	{
		// short up
		//-----
		delay1ms(100);
		// key_sendLin(key_g, key_up);

		// led_set(key_g,0);
		//-----
		opare_flag = 0x00;
	}
	else if (key_mode_g == 4)
	{
		// long up
		//-----
		delay1ms(100);
		/// key_sendLin(key_g, key_long_u);
		//-----
		opare_flag = 0x00;
	}
}

void key_sub(void)
{
	u8 temp = 0;

	temp = GetKey_mode();
	if (key_reg_out > 0)
	{
		key_work(key_reg_out, temp);
		key_reg_out = 0;
	}
}
