#include "app_protocol.h"
#include "../Source/Bsp/bsp_usart.h"
#include "../Source/Dev/dev_config.h"

// 函数声明
static uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len);
static void app_protocol_panel_check(usart1_rx_buf_t *buf);

static dev_panel_callback panel_callback = NULL;
void app_protocol_callback(dev_panel_callback callback)
{
    panel_callback = callback;
}

static panel_frame_t my_panel_frame;
static panel_info_t my_panel_info;
void app_protocol_init(void)
{
    bsp_usart1_rx_callback(app_protocol_panel_check);
}

// 检查数据合法性
static void app_protocol_panel_check(usart1_rx_buf_t *buf)
{
    if (!buf || buf->length < 4) return; // 最小帧长度 FF AA ?? 0D 0A

    uint16_t frame_len = 0; // 完整帧长度

    uint16_t i = 0; // 扫描索引,查找帧头
    uint16_t j = 0; // 查找帧尾索引

    for (i = 0; i + 1 < buf->length; i++) { // 查找帧头
        if (buf->buffer[i] == PANEL_FRAME_RX_HEAD_1 && buf->buffer[i + 1] == PANEL_FRAME_RX_HEAD_2) {
            break; // 找到帧头
        }
    }
    if (i + 1 >= buf->length) { // 没找到帧头,整包数据无效
        buf->length = 0;
        return;
    }
    for (j = i + 2; j + 1 < buf->length; j++) { // 从帧头的下一个字节开始查找帧尾
        if (buf->buffer[j] == PANEL_FRAME_RX_TAIL_1 && buf->buffer[j + 1] == PANEL_FRAME_RX_TAIL_2) {
            break; // 找到完整帧尾
        }
    }
    if (j + 1 >= buf->length) { // 没找到完整帧尾,整包数据无效
        buf->length = 0;
        return;
    }

    frame_len = j + 2 - i; // 帧长度 = (最后一个字节索引) - (帧头索引) + 1,最后一个字节为 j + 1(帧尾第二字节)  => frame_len = (j + 1) - i + 1 = j + 2 - i

    memmove(&buf->buffer[0], &buf->buffer[i], frame_len);
    buf->length = frame_len; // 更新有效帧长度

    if (buf->buffer[0] != PANEL_FRAME_RX_HEAD_1 || buf->buffer[1] != PANEL_FRAME_RX_HEAD_2) {
        APP_ERROR("panel frame");
        return;
    }
    if (buf->length > UART1_RECV_SIZE) {
        APP_ERROR("panel frame too long");
        return;
    }
    if (app_panel_frame_crc(&buf->buffer[4], buf->buffer[2]) != buf->buffer[buf->length - 3]) {
        APP_ERROR("panel frame crc");
        return;
    }
    // APP_PRINTF_BUF("buf", buf->buffer, buf->length);

    // 查找本设备的 addr bl reserve
    const uint8_t my_addr = dev_get_config()->dev_addr;

    uint8_t length   = buf->buffer[2];
    uint8_t sub_idx  = my_addr / PANEL_FRAME_RX_ADDR_LEN; // 在哪个 sub_frame 中
    uint8_t addr_idx = my_addr % PANEL_FRAME_RX_ADDR_LEN; // 在该 sub_frame 中的第几个地址

    uint8_t sub_start = 4 + sub_idx * PANEL_FRAME_RX_SUB_LEN; // 子帧起始地址索引

    memset(&my_panel_info, 0, sizeof(my_panel_info));
    my_panel_info.addr  = buf->buffer[sub_start + addr_idx];
    my_panel_info.bl    = buf->buffer[sub_start + 8];
    my_panel_info.res_1 = buf->buffer[sub_start + 9];
    my_panel_info.res_2 = buf->buffer[sub_start + 10];

    APP_PRINTF("my_panel_info:%02X\n", my_panel_info.addr);

    if (panel_callback) { // 回调到 dev_panel 设备处理
        panel_callback(&my_panel_info);
    }
}

// 组帧上报
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

    my_panel_frame.data[0]  = PANEL_FRAME_TX_HEAD;
    my_panel_frame.data[1]  = PANEL_FRAME_TX_TYPE;
    my_panel_frame.data[2]  = PANEL_FRAME_TX_DATA_LEN;
    my_panel_frame.data[3]  = dev_addr;
    my_panel_frame.data[4]  = level;
    my_panel_frame.data[5]  = status;
    my_panel_frame.data[6]  = key_num;
    my_panel_frame.data[7]  = 0x00; // reserve_1
    my_panel_frame.data[8]  = 0x00; // reserve_2
    my_panel_frame.data[9]  = app_panel_frame_crc(&my_panel_frame.data[3], my_panel_frame.data[2]);
    my_panel_frame.data[10] = 0x0D;
    my_panel_frame.data[11] = 0x0A;

    my_panel_frame.length = 12;
    APP_PRINTF_BUF("tx", my_panel_frame.data, my_panel_frame.length);
    bsp_usart_tx(my_panel_frame.data, my_panel_frame.length);
}

static uint8_t app_panel_frame_crc(uint8_t *rxbuf, uint8_t len)
{
    uint8_t i, sum = 0;
    for (i = 0; i < len; i++) sum = sum + rxbuf[i];
    return (0xff - sum + 1);
}
