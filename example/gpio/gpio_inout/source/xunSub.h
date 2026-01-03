#ifndef __XUNSUB_H
#define __XUNSUB_H
#include "sys.h"
#include "key.h"
#include "led.h"
#include "RelayAPP.h"

/// 函数和间隔自行调整
#define JG_time1 50000
#define JG_sub1() xunSub_nc() //

#define JG_time2 1
#define JG_sub2()       \
    key_sub();          \
    UART1_ReadBoxSUB(); \
    shou_work_sub()

#define JG_time3 10
#define JG_sub3() \
    pw_work();    \
    bg595_shua()

#define JG_time4 1000
#define JG_sub4() bg_led()

#define JG_time5 50
#define JG_sub5() BgBit_sub()

#define JG_time6 10
#define JG_sub6() BgShan_sub()

#define JG_time7 100
#define JG_sub7()  \
    shou_sub();    \
    qingqiu_sub(); \
    xybg_sub()

#define JG_time8 50000
#define JG_sub8() xunSub_nc()

#define JG_time9 50000
#define JG_sub9() xunSub_nc()

#define JG_time10 50000
#define JG_sub10() xunSub_nc()

////appsub head///////
void xunSub_timer(u16 mme);
void xunSub_diao(void);
void xunSub_nc(void);

#endif
