#ifndef __RelayApp_H
#define __RelayApp_H
#include "sys.h"
#define dingzhi_sle 0 // 0=默认自己主机协议 1=尊宝面板协议
#define u8 unsigned char
#define dingzhi_yanshi 0 /// 设置为演示

/*设备头0*/
#define sb_head 0x2a
/*设备标识1*/
#define sb_bgBit 0x01   // 背光码
#define sb_keyaddr 0x02 // 按键码
#define sb_air 0x03     // 空调码
#define sb_card 0x04    // 取电码
#define sb_SetDk 0x05   // 设置灯控地址
/*按键地址2*/

/*按键地址3*/
#define key_Down 0x00   // 按下
#define key_up 0x01     // 弹起
#define key_long_d 0x03 // 长按下
#define key_long_u 0x04 // 长弹起

/*其它地址*/
/*校验*/
void GetDataWork(u8 *Buf);
void BgBit_sub(void);
void key_send(u8 keys, u8 dw);
void BgShan_sub(void);
void BgShan_en(void);
void dk485_init(void);
void ruo_yanshi(u8 keys);

void key_sendLin(u8 keys, u8 dw);
void shou_send(void);
void qingqiu_send(void);
void qingqiu_sub(void);

#endif
