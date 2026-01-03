#include "usart.h"
#include "RelayAPP.h"






extern Pro_flag_s MCU_flag;



////接收中断
void RxIntCallback(void)
{
    u8 temp;
    temp = M0P_UART1->SBUF;
    UART1_R_Interr(temp,StackLen_Value);

}

void uart1_init(void)
{
    //CH
    io_init(p3, pin4, io_tuiwan);
	App_UartInit();
    CH_L;
}	


void  uart1_send(uchar dat)
{
   
   //Uart_SendData(UARTCH1, dat); //启动UART1发送第一个字节
}



