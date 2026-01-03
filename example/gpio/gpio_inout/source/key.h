#ifndef __key_H
#define __key_H
#include "sys.h"
/*******************************************************
输入端口统一设定

********************************************************/
	//led1=pd2 led2=pc6
	//key1=pa2 key2=pa1 key3=pc4 key4=pc5 key5=pd4
	//mos=pd3
	//tx=pd6 rx=pd5
	

#define k1_re             GPIO_PA02_READ() 
#define k2_re             GPIO_PA01_READ() 
#define k3_re             GPIO_PC04_READ() 
#define k4_re             GPIO_PC05_READ() 
#define k5_re             GPIO_PD04_READ() 




#define key_short_down		6
#define	key_long_down		1000


void key_init(void);


void key_sub(void);


#endif

