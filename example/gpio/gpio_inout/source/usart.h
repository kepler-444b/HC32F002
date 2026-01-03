#ifndef __USART_H
#define __USART_H
#include "sys.h"








void uart1_init(void);

void uart1_send(uchar dat);
void UART1_SendBox(u8 *buf,u8 len);
void UART1_ReadBoxSUB(void);
void UART1_R_Interr(u8 datas,u8 StackLen);


void UART1_try(void);


#endif	   
















