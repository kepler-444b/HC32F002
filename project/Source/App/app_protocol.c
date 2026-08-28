#include "app_protocol.h"
#include <stdbool.h>
#include "../Source/Bsp/bsp_usart.h"
#include "../Source/App/app_timer.h"
#include "../Source/Dev/dev_config.h"
#include "../Source/Bsp/bsp_595/bsp_595.h"

// 设备重启命令
static uint8_t reset_cmd[6] = {0xFE, 0x72, 0x65, 0x73, 0x65, 0x74};
// 设置单个地址
static uint8_t set_addr_head[4] = {0X0E, 0X0E, 0X0E, 0X0E}; // 批量设置设备地址帧头

// 函数声明
static uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len);
static void app_protocol_panel_check(usart1_rx_buf_t *buf);
static void app_delay_apply_addr(void *arg);

static dev_protocol_callback protocol_callback = NULL;
static dev_event_callback event_callback       = NULL;

void app_protocol_callback(dev_protocol_callback callback)
{
    protocol_callback = callback;
}

void app_evnet_callback(dev_event_callback callback)
{
    event_callback = callback;
}

static panel_frame_t my_panel_frame = {0};
static panel_info_t my_panel_info   = {0};

static bool apply_addr;

void app_protocol_init(void)
{
    apply_addr = false;
    bsp_usart1_rx_callback(app_protocol_panel_check);
}

// 检查数据合法性
static void app_protocol_panel_check(usart1_rx_buf_t *buf)
{
    uint8_t data_type     = buf->buffer[3];
    uint16_t data_length  = buf->buffer[2];
    const uint8_t my_addr = dev_get_config()->dev_addr;

    switch (data_type) {
        case SET_STATE: { // 设置面板状态

            uint8_t length   = buf->buffer[2];
            uint8_t sub_idx  = my_addr / PANEL_FRAME_RX_ADDR_LEN; // 在哪个 sub_frame 中
            uint8_t addr_idx = my_addr % PANEL_FRAME_RX_ADDR_LEN; // 在该 sub_frame 中的第几个地址

            APP_PRINTF("sub_idx:%d addr_idx:%d", sub_idx, addr_idx);
            APP_PRINTF("my_addr:%d sub_idx:%d addr_idx:%d\n", my_addr, sub_idx, addr_idx);
            uint8_t sub_start = 4 + sub_idx * PANEL_FRAME_RX_SUB_LEN; // 子帧起始地址索引

            memset(&my_panel_info, 0, sizeof(my_panel_info));
            my_panel_info.addr    = buf->buffer[sub_start + (addr_idx * 2)];
            my_panel_info.reserve = buf->buffer[sub_start + (addr_idx * 2) + 1];

            my_panel_info.bl    = buf->buffer[sub_start + 16];
            my_panel_info.res_1 = buf->buffer[sub_start + 17];
            my_panel_info.res_2 = buf->buffer[sub_start + 18];

            APP_PRINTF("my_panel_info:%02X\n", my_panel_info.addr);

            if (protocol_callback) { // 回调到 dev_panel 设备处理
                protocol_callback(&my_panel_info);
            }
        } break;
        case SET_ADDR_SINGLE: { // 接收到了上位机对"单发地址"的回复
            if (!apply_addr) return;
            if (data_length != 3) return;
            if (buf->buffer[4] != 0x1B || buf->buffer[5] != 0x2C) return;

            uint8_t addr = buf->buffer[6];
            static event_t temp_event;
            temp_event.data   = &addr;
            temp_event.length = sizeof(addr);

            if (event_callback) {
                event_callback(SET_ADDR, &temp_event);
            }
            break;
        }
        case SET_ADDR_BATCH: { // 群发地址
            APP_PRINTF_BUF("buf", buf->buffer, buf->length);
            if (data_length != 3) return;
            uint8_t addr = buf->buffer[6];
            static event_t temp_event;
            temp_event.data   = &addr;
            temp_event.length = sizeof(addr);

            if (event_callback) {
                event_callback(SET_ADDR, &temp_event);
            }
            break;
        }
        case SET_DEV_RESET: { // 设备重启
            if (data_length != 1) return;
            if (buf->buffer[4] == 0x00) { // 重启的设备是面板
                event_callback(SET_RESET, NULL);
            }
            break;
        }
        case SET_DEG_INFO: { // 设备信息
            if (buf->buffer[6] != my_addr) return;
            if (data_length != 4) return;

            if (buf->buffer[4] == 0x00) { // 获取信息的设备是面板
                event_callback(GET_INFO, NULL);
                app_send_to_software(SET_DEG_INFO);
            }
            break;
        }
        default:
            break;
    }
}

// 组帧上报给上位机
void app_protocol_build(uint8_t level, uint8_t status, uint8_t key_num)
{
    if (key_num > KEY_NUMBER) {
        return;
    }
    if (level > 1) {
        APP_ERROR("level reeor");
        return;
    }
    uint8_t dev_addr = dev_get_config()->dev_addr;
    if (dev_addr == 0xFF) {
        APP_ERROR("dev_addr error");
        return;
    }
    memset(&my_panel_frame, 0, sizeof(my_panel_frame));

    my_panel_frame.data[0] = PANEL_FRAME_TX_HEAD;
    my_panel_frame.data[1] = PANEL_FRAME_TX_TYPE;
    my_panel_frame.data[2] = PANEL_FRAME_TX_DATA_LEN;
    my_panel_frame.data[3] = dev_addr; // 面板地址
    my_panel_frame.data[4] = level;    // 触发类型
    my_panel_frame.data[5] = status;   // 面板状态
    my_panel_frame.data[6] = key_num;  // 按键号

    my_panel_frame.data[7]  = 0x00; // reserve_1
    my_panel_frame.data[8]  = 0x00; // reserve_2
    my_panel_frame.data[9]  = app_panel_frame_crc(&my_panel_frame.data[3], my_panel_frame.data[2]);
    my_panel_frame.data[10] = 0x0D;
    my_panel_frame.data[11] = 0x0A;

    my_panel_frame.length = 12;
    bsp_usart_tx(my_panel_frame.data, my_panel_frame.length);
}

void app_send_to_software(uint8_t type)
{
    uint8_t send_array[9];
    send_array[0] = 0xFE;
    send_array[1] = 0xBB;
    send_array[2] = 0x03;

    send_array[7] = 0x0D;
    send_array[8] = 0x0A;

    if (type == SET_ADDR_SINGLE) { // 单发地址
        send_array[3] = type;
        send_array[4] = 0xB1;
        send_array[5] = 0xB2;
        send_array[6] = 0xB3;
        apply_addr    = true; // 本设备发送了申请地址请求
        app_timer_start(500, app_delay_apply_addr, false, NULL, "apply");
    } else if (type == SET_DEG_INFO) {
        send_array[3] = type;
        send_array[4] = 0x00;    // 设备类型,面板为0x00
        send_array[5] = DEV_VER; // 软件版本
        send_array[6] = dev_get_config()->dev_addr;
    }
    bsp_usart_tx(send_array, sizeof(send_array));
}

static void app_delay_apply_addr(void *arg)
{
    apply_addr = false; // 等待上位机回复的窗口期
    APP_PRINTF("apply_addr is false\n");
}

static uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len)
{
    uint8_t i, sum = 0;
    for (i = 0; i < len; i++) sum = sum + rxbuf[i];
    return (0xff - sum + 1);
}
