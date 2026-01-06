#ifndef _APP_PROTOCOL_H_
#define _APP_PROTOCOL_H_
#include <stdint.h>

// 面板上报数据帧相关宏定义
#define PANEL_FRAME_TX_HEAD     0xAA // 固定帧头
#define PANEL_FRAME_TX_TYPE     0x01 // 报文类:面板类暂固定为0x01;
#define PANEL_FRAME_TX_DATA_LEN 6    // 面板类上报有效数据长度固定为6
#define PANEL_FRAME_TX_MAX_LEN  24

// 主机下发数据帧相关宏定义
#define PANEL_FRAME_RX_HEAD_1   0xFF // 固定帧头
#define PANEL_FRAME_RX_HEAD_2   0xAA // 固定帧头
#define PANEL_FRAME_RX_SUB_LEN  11   // 子帧长度
#define PANEL_FRAME_RX_ADDR_LEN 8    // 子帧中的地址个数

// 设置软件下发地址
#define PANEL_SET_ADDR_HEAD      0X0E
#define PANEL_SET_ADDR_TAIL      0xB0
#define PANEL_SET_ADDR_FRAME_LEN 0x22

#define PANEL_FRAME_RX_TAIL_1    0x0D
#define PANEL_FRAME_RX_TAIL_2    0x0A

typedef enum {

    SET_ADDR,
} event_e;

typedef struct {
    uint8_t *data;
    uint8_t length;
} event_t;

typedef struct
{
    uint8_t data[PANEL_FRAME_TX_MAX_LEN];
    uint16_t length;
} panel_frame_t;

typedef struct
{
    uint8_t addr;
    uint8_t bl;
    uint8_t res_1;
    uint8_t res_2;

} panel_info_t;

typedef void (*dev_protocol_callback)(panel_info_t *);
void app_protocol_callback(dev_protocol_callback callback);

typedef void (*dev_event_callback)(event_e event, event_t *event_data);
void app_evnet_callback(dev_event_callback callback);

void app_protocol_init(void);
void app_send_to_software(void);
void app_protocol_build(uint8_t level, uint8_t status, uint8_t key_num);

#endif