#ifndef _DEV_MANAGER_H_
#define _DEV_MANAGER_H_

#define PANEL // 灯控面板

#if defined PANEL
#define PANEL_A13
#define KEY_NUMBER 6
#endif

void dev_jump_dev(void);

#endif