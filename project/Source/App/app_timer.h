#ifndef _APP_TIMER_H_
#define _APP_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
void app_timer_init(void);

#define MAX_SOFT_TIMERS    5  ///< 最大支持的软定时器数量
#define MAX_TIMER_NAME_LEN 10 ///< 定时器名称最大长度

typedef enum {
    TIMER_STATE_INACTIVE = 0, ///< 定时器未激活
    TIMER_STATE_ACTIVE,       ///< 定时器激活运行中
    TIMER_STATE_PENDING       ///< 定时器回调待执行
} timer_state_e;

typedef enum {
    TIMER_ERR_SUCCESS = 0,   ///< 操作成功
    TIMER_ERR_INVALID_PARAM, ///< 参数无效
    TIMER_ERR_NO_RESOURCE,   ///< 资源不足
    TIMER_ERR_NOT_FOUND,     ///< 定时器未找到
    TIMER_ERR_NAME_TOO_LONG  ///< 定时器名称过长
} timer_error_e;

/**
 * @brief 定时器回调函数类型
 * @param arg 用户自定义参数
 */
typedef void (*SoftTimerCallback)(void *arg);

typedef struct
{
    volatile timer_state_e state;  ///< 定时器状态（中断/主程序共享）
    volatile uint32_t start_time;  ///< 定时器启动时间（中断读取，poll更新）
    bool repeat;                   ///< 是否循环定时
    uint32_t interval_ms;          ///< 定时器间隔(ms)
    SoftTimerCallback callback;    ///< 回调函数
    void *user_arg;                ///< 用户参数
    char name[MAX_TIMER_NAME_LEN]; ///< 定时器名称
} soft_timer_t;

// 初始化定时器模块
void app_timer_init(void);

// 开启一个定时器
timer_error_e app_timer_start(uint32_t interval_ms, SoftTimerCallback callback, bool repeat, void *arg, const char *name);

// 通过名称停止定时器
timer_error_e app_timer_stop(const char *name);

// 通过名称检查定时器是否激活
bool app_timer_is_active(const char *name);

// 处理定时器回调(主循环调用)
void app_timer_poll(void);

// 获取系统运行时间
uint32_t app_timer_get_ticks(void);

#endif