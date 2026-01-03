#include "led.h"
#include "key.h"

extern Pro_flag_s MCU_flag;
extern u8 Shan_flag;

//Led对应位
u8 LedSX_tab[16]=
{
	Lb1_0,Lb2_0,Lb3_0,Lb4_0,Lb5_1,Lb6_1,
	Lc1_0,Lc2_0,Lc3_0,Lc4_0,Lc5_1,Lc6_1,
	k01_1,k02_1,k03_1,k04_1
};
//BUF对应位
u8 LedBF_tab[16]=
{
	1,1,1,1,0,0,
	1,1,1,1,0,0,
	0,0,0,0
};

void LED_Init(void)
{
    
}
u8 h595_buf[2];
void h595d_write(u8 *buf,u8 num)
{
	u8 temp,i,j;
	//r_clk_pc=0;
    r_clk_L;
	delay_us(1);
	for(i=0;i<num;i++)
	{
		temp=buf[i];
		for(j=0;j<8;j++)
		{
			//st_clk_pc=0;
            st_clk_L;
			delay_us(1);
			if(temp&0x80)
			{
				//seg_pc=1;	
                seg_H;
			}
			else
			{
				//seg_pc=0;
                seg_L;
			}
			delay_us(1);
			//st_clk_pc=1;
            st_clk_H;
			delay_us(1);
			temp=temp<<1;
		}
	}
	
	delay_us(1);
	//r_clk_pc=1;
    r_clk_H;
	delay_us(1);
    r_clk_L;
	//r_clk_pc=0;
}

void bg595_ceshi(u8 *buf)
{
	if((buf[0]==0x15)&(buf[1]==0x51))
	{
		h595_buf[0]=buf[2];
		h595_buf[1]=buf[3];
		h595d_write(h595_buf, 2);
	}
	else if((buf[0]==0x15)&(buf[1]==0x52))
	{
		
		MCU_flag.led[0]=buf[2];
		MCU_flag.led[1]=buf[3];
		MCU_flag.led[2]=buf[4];
		MCU_flag.led[3]=buf[5];
		MCU_flag.led[4]=buf[6];
		MCU_flag.led[5]=buf[7];
		
		h595d_write(h595_buf, 2);
	}
	else if((buf[0]==0x15)&(buf[1]==0x53))
	{
		UART1_SendBox(h595_buf,2);
		
	}
}
void bg595_shua(void)
{
	u8 i=0;
	if(Shan_flag==1)
	{
		h595d_write(h595_buf, 2);
		return;
	}
	
		i=0;
		if((MCU_flag.guanLed&0x01)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		i=1;
		if((MCU_flag.guanLed&0x02)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		i=2;
		if((MCU_flag.guanLed&0x04)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		i=3;
		if((MCU_flag.guanLed&0x08)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		i=4;
		if((MCU_flag.guanLed&0x10)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		i=5;
		if((MCU_flag.guanLed&0x20)>0)
		{led_set(i+1,0);led_set(i+7,0);}
		else
		{
			if(MCU_flag.led[i]>0)
			{led_set(i+1,1);led_set(i+7,0);}
			else
			{led_set(i+1,0);led_set(i+7,1);}
		}
		if(MCU_flag.key_s==3)
		{//横向三键-左侧背光灭掉1、5、4
			i=0;
			led_set(i+1,0);led_set(i+7,0);
			i=4;
			led_set(i+1,0);led_set(i+7,0);
			i=3;
			led_set(i+1,0);led_set(i+7,0);
		}
		if(MCU_flag.key_s==2)
		{//横向2键-左侧背光灭掉1、4
			i=0;
			led_set(i+1,0);led_set(i+7,0);
			i=3;
			led_set(i+1,0);led_set(i+7,0);
		}
	h595d_write(h595_buf, 2);
	
}



void led_set(u8 relay_g,u8 open_g)
{
	if((relay_g>0)&(relay_g<=16))
	{
		if(open_g==0)
		{
			h595_buf[LedBF_tab[relay_g-1]]&=~LedSX_tab[relay_g-1];
		}
		else
		{
			h595_buf[LedBF_tab[relay_g-1]]|=LedSX_tab[relay_g-1];
		}
	}
}


u8 led_read(u8 relay_g)
{
	if((relay_g>0)&(relay_g<=24))
	{
		if((h595_buf[LedBF_tab[relay_g-1]]&LedSX_tab[relay_g-1])>0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/*
void led_Conall(uchar led_g,uchar open_g)
{
	if(open_g==0)
	{
		if((led_g&0x01)>0)
			led_set(1,0);
		if((led_g&0x02)>0)
			led_set(2,0);
        if((led_g&0x04)>0)
			led_set(3,0);
		if((led_g&0x08)>0)
			led_set(4,0);
        if((led_g&0x10)>0)
			led_set(5,0);
        if((led_g&0x20)>0)
			led_set(6,0);
		if((led_g&0x40)>0)
			led_set(7,0);
	}
	else
	{
		if((led_g&0x01)>0)
			led_set(1,1);
		if((led_g&0x02)>0)
			led_set(2,1);
        if((led_g&0x01)>0)
			led_set(3,1);
		if((led_g&0x02)>0)
			led_set(4,1);
        if((led_g&0x10)>0)
			led_set(5,1);
        if((led_g&0x20)>0)
			led_set(6,1);
		if((led_g&0x40)>0)
			led_set(7,1);
	}
}

*/
/*弱背光调节*/
u8 pw_bfb[7]={0};
u8 tg_ji=0;
void pwTimer_sub(void)
{
	tg_ji++;
	if(bg_re()>0)
			return;
	/*
    if(MCU_flag.led[6]==0)
    {
    	led_set(7,0);led_set(8,0);led_set(9,0);
		led_set(10,0);led_set(11,0);led_set(12,0);
    }
    else if(MCU_flag.led[6]==100)
    {
    	led_set(7,1);led_set(8,1);led_set(9,1);
		led_set(10,1);led_set(11,1);led_set(12,1);
    }
	else if((MCU_flag.led[6]<=98)&(MCU_flag.led[6]>0))
    {
		led_set(7,1);led_set(8,1);led_set(9,1);
		led_set(10,1);led_set(11,1);led_set(12,1);
    }
	*/
    if(tg_ji>98)
        tg_ji=0;
}

/*背光缓灭*/
u8 slj=0;//速率调节
u8 pw_jishu[7];
void pw_work(void)
{
    u8 i=0;
    slj++;
    if(slj<1)
        return;
    slj=0;
    for(i=0;i<7;i++)
    {
        if(pw_bfb[i]>pw_jishu[i])
        {
            pw_bfb[i]--;
			if(MCU_flag.shou_en==1)
			{//检测背光灭了等1s开启雷达
				if(pw_bfb[6]==0)
					shou_stopEN(20); 
				
			}
        }
        else if(pw_bfb[i]<pw_jishu[i])
        {
            pw_bfb[i]++;
        }
    }
    

  
        
}


/*led百分比设置*/
void ledB_set(uchar led_No,uchar bfb)
{/*先把灯的开关状态给缓存*/
	if(led_No<=7)
	{
		MCU_flag.led[led_No-1]=bfb;
	}
	else
	{
		
	}
}

u16 bg_delay=0;
void bg_en(void)
{
    //bg_delay=5*MCU_flag.yanchi;
    ledB_set(7,100);
}
/*bg_con背光控制*/
u32 bg_con(void)
{
	u32 cc=(MCU_flag.bgen24to32<<24)+(MCU_flag.bgen16to23<<16)+(MCU_flag.bgen8to15<<8)+MCU_flag.bgen0to7;
	return ((1<<MCU_flag.mb_addr)&cc);
}
u8 bg_re()
{
	if((MCU_flag.led[0]==0)&(MCU_flag.led[1]==0)&(MCU_flag.led[2]==0)&(MCU_flag.led[3]==0)&(MCU_flag.led[4]==0)&(MCU_flag.led[5]==0))
		return 0;
	else
		return 1;
}
void bg_led(void)
{
    //if((bg_delay>0)&(pw_jishu[0]==0)&(pw_jishu[1]==0)&(pw_jishu[2]==0)
      //  &(pw_jishu[3]==0)&(pw_jishu[4]==0)&(pw_jishu[5]==0))
    if(bg_delay>0)
    {
        bg_delay--;
        if(bg_delay==0)
        {
        	if(MCU_flag.shou_en==1)
			{
				shou_stopEN(20); 
				if(bg_re()==0)
					ledB_set(7,0);
			}
			else
            {
            	if(bg_con()==0)
            	{
            		if(bg_re()==0)
            		ledB_set(7,MCU_flag.lv);
				}
				else
				{
					if(bg_re()==0)
					ledB_set(7,0);
				}
			}
        }
    }
}
/*收到协议后，改变背光*/



//协议延迟灭背光
u16 xybg_num=0;
void xybg_sub(void)
{
   	if(xybg_num>0)
   	{
   		xybg_num--;
		if(xybg_num==0)
			MCU_flag.guanLed=0x3f;
   	}
	
			
}

void xybg_led(void)
{
    
	if(bg_con()==0)
	{
		MCU_flag.guanLed=0x0;
	}
	else
	{
		//加延迟后灭背光
		//MCU_flag.guanLed=0x3f;
		if(MCU_flag.yanchi==0)
			xybg_num=5*10;
		else if(MCU_flag.yanchi>200)
			MCU_flag.guanLed=0x3f;
		else
			xybg_num=MCU_flag.yanchi*9;
	}
			
}


/*手感应延迟触发*/
u8 shou_ji=0;/*0.1s*/
void shou_stopEN(u8 delay100ms)
{
	MCU_flag.shou_stop=1;
	shou_ji=delay100ms;
}




/*手感应操作函数*/
u8 shou_num=0;

void shou_sub(void)
{
    if(shou_ji>0)
    {
    	shou_ji--;
		if(shou_ji==0)
		{
			MCU_flag.shou_stop=0;
		}
    }
}



#define caiyang				15
#define caiyang_bi 			13

u8 ex;
u8 ex_f;
/*手感应接口信号处理*/
u8 exter_num=0;
///u8 ex_shangdian=2;/*上电初始化，前两轮采样不算*/
void shou_work_sub(void)
{
    
    if(MCU_flag.shou_en!=1)
		return;
	if(MCU_flag.shou_stop==1)//stop手感应触发
		return;
	exter_num++;
    
    if(Shou_re==1)
        ex++;
     
	if(exter_num>caiyang)
	{
		exter_num=0;
		if(ex>caiyang_bi)
		{
			if(ex_f==0)
			{	
				//高到低--1 ------感应触发
				shou_send();
				bg_en();
			}
				
			ex_f=1;
		}	
		else
		{
			if(ex_f==1)
			{
				//低到高--1 ------感应未触发
				    
			}
				
			ex_f=0;
		}
		ex=0;
	}
}




//4键和6键的硬件  共用程序，只有四位做2键不共用
u8 Get_set(void)
{
	u8 temp=0;
	delay_ms(50);
	if(S1_re==0)
		temp|=0x1;
	if(S2_re==0)
		temp|=0x2;
	//if(S3_re==0)
		//temp|=0x4;

	if((temp&0x01)==0x01)
	{
			MCU_flag.key_s=1;
			if(S3_re==0)
			    MCU_flag.key_s=1;/*四键做两键开关*/
			else
			    MCU_flag.key_s=2;/*四键做两键开关*/
	}
	else if((temp&0x02)==0x02)
		{MCU_flag.key_s=3;}
	else
		{MCU_flag.key_s=6;}
	if(dingzhi_yanshi==1)
		MCU_flag.key_s=6;
	return temp;
}







