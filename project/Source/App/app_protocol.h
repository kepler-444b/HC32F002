#ifndef _APP_PROTOCOL_H_
#define _APP_PROTOCOL_H_
#include <stdint.h>

// 面板上报主机数据帧相关宏定义
#define PANEL_FRAME_TX_HEAD     0xAA // 固定帧头
#define PANEL_FRAME_TX_TYPE     0x01 // 报文类:面板类暂固定为0x01;
#define PANEL_FRAME_TX_DATA_LEN 6    // 面板类上报有效数据长度固定为6
#define PANEL_FRAME_TX_MAX_LEN  24

#define PANEL_FRAME_RX_SUB_LEN  19 // 子帧长度
#define PANEL_FRAME_RX_ADDR_LEN 8  // 子帧中的地址个数

#define SET_STATE               0x01 // 设置状态
#define SET_ADDR_SINGLE         0x02 // 单发地址
#define SET_ADDR_BATCH          0x03 // 群发地址
#define SET_DEV_RESET           0x04 // 设备重启
#define SET_DEG_INFO            0x05 // 设备信息

typedef enum {

    SET_ADDR,
    SET_RESET,
    GET_INFO,
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
    uint8_t addr;    // 每个按键状态
    uint8_t reserve; // 保留
    uint8_t bl;      // 背光状态
    uint8_t res_1;   // 保留1(亮度值,对于灯控面板暂时无用)
    uint8_t res_2;   // 保留2(色温值,对于灯控面板暂时无用)

} panel_info_t;

typedef void (*dev_protocol_callback)(panel_info_t *);
void app_protocol_callback(dev_protocol_callback callback);

typedef void (*dev_event_callback)(event_e event, event_t *event_data);
void app_evnet_callback(dev_event_callback callback);

void app_protocol_init(void);
void app_send_to_software(uint8_t type);
void app_protocol_build(uint8_t level, uint8_t status, uint8_t key_num);

#endif