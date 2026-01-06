#include "dev_panel.h"
#include "dev_manager.h"
#include "../Source/Bsp/bsp_pcb.h"
#include "../Source/Bsp/bsp_adc.h"
#include "../Source/bsp/bsp_usart.h"
#include "../Source/App/app_timer.h"
#include "../Source/app/app_base.h"
#include "../Source/Dev/dev_config.h"
#include "../Source/App/app_base.h"
#include "../Source/Bsp/bsp_595/bsp_595.h"

#if defined PANEL
#define LONG_PRESS   60  // 长按时间 3s (按键需要填充电压buf,故而每 5*10 ms 检测一次)
#define FILCK_COUNT  100 // 闪烁时间 500ms

#define VOL_BUF_SIZE 10  // 电压值缓冲区数量
#define MIN_VOL      495 // 无按键按下时的最小电压值
#define MAX_VOL      500 // 无按键按下时的最大电压值

typedef struct
{
    uint16_t min;
    uint16_t max;
} key_vol_t;

typedef struct
{
    bool k_press;  // 按键按下
    bool k_status; // 按键状态

    bool w_cur;  // 白灯状态
    bool w_last; // 白灯上次状态

    bool y_cur;  // 黄灯状态
    bool y_last; // 黄灯上次状态

    const key_vol_t vol_range; // 按键电压范围
} panel_status_t;

typedef struct
{
    uint8_t key_status; // 面板状态
    bool bl_close;      // 关闭背光
    bool bl_open;       // 开启背光
    bool bl_status;

    bool led_filck;           // 闪烁
    bool key_long_press;      // 长按状态
    bool enter_config;        // 进入配置状态
    uint16_t key_long_count;  // 长按计数
    uint16_t led_filck_count; // 闪烁计数
} common_panel_t;

typedef struct
{
    uint8_t buf_idx; // 缓冲区下标
    uint16_t vol;    // 电压值
    uint16_t vol_buf[VOL_BUF_SIZE];
} adc_value_t;

static adc_value_t my_adc_value;
static common_panel_t my_common_panel;
static panel_status_t my_panel_status[KEY_NUMBER] = {
    PANEL_VOL_RANGE_DEF,
};

// 函数声明
static void dev_panel_read_adc(void *arg);
static uint8_t dev_panel_key_to_bit(void);
static void dev_protocol_parse(panel_info_t *info);
static void dev_event_parse(event_e event, event_t *event_data);
static void dev_panel_bit_to_key(uint8_t key_bit);

void dev_panel_init(void)
{
    bsp_pcb_panel_init();
    bsp_adc_init();
    bsp_595_init();
    bsp_595_output(0x00, 0xFF);
    app_timer_start(5, dev_panel_read_adc, true, NULL, "read_adc");

    dev_config_load();
    app_protocol_callback(dev_protocol_parse);
    app_evnet_callback(dev_event_parse);
}

static void dev_panel_read_adc(void *arg)
{
    uint16_t adc_value = bsp_get_adc();
    // APP_PRINTF("%d\n", adc_value);
    for (uint8_t i = 0; i < KEY_NUMBER; i++) { // 处理按键

        if (adc_value < my_panel_status[i].vol_range.min || adc_value > my_panel_status[i].vol_range.max) {
            if (adc_value >= MIN_VOL && adc_value <= MAX_VOL) {
                my_panel_status[i].k_press = false;

                my_common_panel.key_long_press = false;
                my_common_panel.key_long_count = 0;
            }
            continue;
        }
        my_adc_value.vol_buf[my_adc_value.buf_idx++] = adc_value;
        if (my_adc_value.buf_idx < VOL_BUF_SIZE) {
            continue;
        }
        my_adc_value.buf_idx = 0; // vol_buf is full

        uint16_t new_value = app_calculate_average(my_adc_value.vol_buf, VOL_BUF_SIZE);
        if (new_value < my_panel_status[i].vol_range.min || new_value > my_panel_status[i].vol_range.max) {
            continue; // 检查平均值是否在有效范围
        }
        if (!my_panel_status[i].k_press && !my_common_panel.enter_config) { // 处理按键按下
            my_panel_status[i].k_status ^= 1;

            my_common_panel.key_status = dev_panel_key_to_bit();
            app_protocol_build(0, my_common_panel.key_status, i);
            delay1ms(50);
            my_panel_status[i].k_press = true;

            my_common_panel.key_long_press = true;
            my_common_panel.key_long_count = 0;
        }
        if (my_common_panel.key_long_press && ++my_common_panel.key_long_count >= LONG_PRESS) { // 处理长按
            APP_PRINTF("long press\n");
            app_send_to_software();
            my_common_panel.key_long_press = false;
        }
    }

    if (my_common_panel.led_filck) { // 处理闪烁
        if (my_common_panel.led_filck_count == 0) {
            bsp_595_output(0xFF, 0x00);
        }
        my_common_panel.led_filck_count++;
        if (my_common_panel.led_filck_count >= FILCK_COUNT) {
            bsp_595_output(0x00, 0xFF);
            my_common_panel.led_filck       = false;
            my_common_panel.led_filck_count = 0;
        }
    }
}

// 打包按键状态为位字段
static uint8_t dev_panel_key_to_bit(void)
{
    uint8_t key_bit = 0;
    for (uint8_t i = 0; i < KEY_NUMBER; i++) {
        key_bit |= (my_panel_status[i].k_status & 1) << i;
    }
    return key_bit;
}

// 根据位字段更新按键状态
static void dev_panel_bit_to_key(uint8_t key_bit)
{
    for (uint8_t i = 0; i < KEY_NUMBER; i++) {
        my_panel_status[i].k_status = !!(key_bit & (1 << i));
    }
}

static void dev_protocol_parse(panel_info_t *info)
{
    uint8_t y_status = (~info->addr);

    bsp_595_output(info->addr, y_status);

    my_common_panel.key_status = info->addr;
    dev_panel_bit_to_key(my_common_panel.key_status);
}

static void dev_event_parse(event_e event, event_t *event_data)
{
    switch (event) {
        case SET_ADDR: {
            uint8_t addr = *(uint8_t *)event_data->data;
            if (addr < PANEL_ADDR_MAX) {
                dev_config_save(&addr, sizeof(addr));
                my_common_panel.led_filck = true;
            } else {
                APP_ERROR("addr is too large");
            }
        } break;
        default:
            return;
    }
}
#endif