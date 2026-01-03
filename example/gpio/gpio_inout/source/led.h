#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
/*******************************************************

********************************************************/


//led1=pd03(20) led2=pa03(10) led3=pb04(12)
//led4=pd04(1) led5=pc03(13) led6=pb05(11)


#define led1_1 GPIO_PD03_SET()
#define led1_0 GPIO_PD03_RESET()
#define led1_re GPIO_PD03_READ()

#define led2_1 GPIO_PA03_SET()
#define led2_0 GPIO_PA03_RESET()
#define led2_re GPIO_PA03_READ()


#define led3_1 GPIO_PB04_SET()
#define led3_0 GPIO_PB04_RESET()
#define led3_re GPIO_PB04_READ()


#define led4_1 GPIO_PD04_SET()
#define led4_0 GPIO_PD04_RESET()
#define led4_re GPIO_PD04_READ()


#define led5_1 GPIO_PC03_SET()
#define led5_0 GPIO_PC03_RESET()
#define led5_re GPIO_PC03_READ()


#define led6_1 GPIO_PB05_SET()
#define led6_0 GPIO_PB05_RESET()
#define led6_re GPIO_PB05_READ()


#define led7_1 GPIO_PA01_SET()
#define led7_0 GPIO_PA01_RESET()
#define led7_re GPIO_PA01_READ()

//s1=pc06(16) s2=pc05(15) s3=pa04(8)
//shou=pd02(19)

#define Shou_re GPIO_PD02_READ()

#define S1_re GPIO_PC06_READ()
#define S2_re GPIO_PC05_READ()
#define S3_re GPIO_PA04_READ()

///
#define st_clk_H				GPIO_PB05_SET()//(_pb, pin5, TRUE)		//ÒÆÎ»¼Ä´æÆ÷
#define	r_clk_H				    GPIO_PA01_SET()//Gpio_SetIO(_pa, pin1, TRUE)		//Ð´Ëø´æÆ÷      
#define	seg_H					GPIO_PC03_SET()//Gpio_SetIO(_pc, pin3, TRUE)		//Êý¾Ý        

#define st_clk_L				GPIO_PB05_RESET()//(_pb, pin5, FALSE)		//ÒÆÎ»¼Ä´æÆ÷
#define	r_clk_L				    GPIO_PA01_RESET()//Gpio_SetIO(_pa, pin1, FALSE)		//Ð´Ëø´æÆ÷      
#define	seg_L					GPIO_PC03_RESET()//Gpio_SetIO(_pc, pin3, FALSE)		//Êý¾Ý     


//buf0 Lc=³ÈÉ«Lb=°×É«
#define Lc1_0					0x01
#define Lc2_0					0x02            
#define Lc3_0					0x04
#define Lc4_0					0x08
#define Lb1_0					0x10
#define Lb2_0					0x20
#define Lb3_0					0x40
#define Lb4_0					0x80

//buf1
#define Lb5_1					0x01
#define Lb6_1					0x02
#define Lc5_1					0x04
#define Lc6_1					0x08
#define k01_1					0x10
#define k02_1					0x20
#define k03_1					0x40
#define k04_1					0x80






void LED_Init(void);
void led_Conall(uchar led_g,uchar open_g);
void pwTimer_sub(void);
void pw_work(void);

void ledB_set(uchar led_No,uchar bfb);
void led_set(u8 relay_g,u8 open_g);


void bg_en(void);
void bg_led(void);

void xybg_sub(void);

void shou_stopEN(u8 delay100ms);

void shou_sub(void);
void shou_work_sub(void);
void shou_send(void);

u8 bg_re(void);
void bg595_shua(void);

void xybg_led(void);

u8 Get_set(void);

void h595d_write(u8 *buf,u8 num);
void bg595_ceshi(u8 *buf);

#endif

















