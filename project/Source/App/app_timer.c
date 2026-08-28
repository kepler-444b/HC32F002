#include "app_timer.h"
#include "sysctrl.h"
#include "btim.h"
#include "../Source/Bsp/bsp_usart.h"

// 函数声明
static int find_free_timer(void);
static int find_timer_by_name(const char *name);

static soft_timer_t my_soft_timer[MAX_SOFT_TIMERS]; // 软定时器数组
static volatile uint32_t system_ticks = 0;          // 系统基准时间(毫秒),定时时间不得超过49.7天

void app_timer_init(void)
{
    stc_btim_init_t stcInitCfg = {0};

    SYSCTRL_FuncEnable(SYSCTRL_FUNC_GTIMCONFIG);
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERIRESET_GTIM);

    stcInitCfg.u32TaskMode = BTIM_TASK_MODE_CONTINUOUS_COUNTER; // 连续计数模式
    stcInitCfg.u32WorkMode = BTIM_WORK_MODE_PCLK;               // 计数时钟源 = PCLK

    stcInitCfg.u32Prescaler = BTIM_COUNTER_CLK_DIV8; // 预分频 8
    stcInitCfg.u32ToggleEn  = BTIM_TOGGLE_DISABLE;   // TOG 输出不使能

    stcInitCfg.u32AutoReloadVal = 3000 - 1; // 自动重载寄存ARR赋值
    Btim_Init(BTIM3, &stcInitCfg);

    Btim_ClearFlag(BTIM3, BTIM_IT_CLR_UI); // 清除溢出中断标志位
    Btim_EnableIT(BTIM3, BTIM_IT_UI);      // 允许BTIM3溢出中断
    EnableNvic(BTIM3_4_5_IRQn, IrqLevel3, TRUE);

    Btim_Enable(BTIM3);
}

timer_error_e app_timer_start(uint32_t interval_ms, SoftTimerCallback callback, bool repeat, void *arg, const char *name)
{
    if (!callback || interval_ms == 0) {
        return TIMER_ERR_INVALID_PARAM;
    }

    if (name && strlen(name) >= MAX_TIMER_NAME_LEN) // 检查名称长度
    {
        return TIMER_ERR_NAME_TOO_LONG;
    }
    int id = find_free_timer();
    if (id >= 0) {
        memset(&my_soft_timer[id], 0, sizeof(soft_timer_t)); // 清零结构体
        my_soft_timer[id].state       = TIMER_STATE_ACTIVE;
        my_soft_timer[id].repeat      = repeat;
        my_soft_timer[id].start_time  = system_ticks;
        my_soft_timer[id].interval_ms = interval_ms;
        my_soft_timer[id].callback    = callback;
        my_soft_timer[id].user_arg    = arg;

        if (name && name[0] != '\0') {
            strncpy(my_soft_timer[id].name, name, MAX_TIMER_NAME_LEN - 1);
            my_soft_timer[id].name[MAX_TIMER_NAME_LEN - 1] = '\0';
        }
    }

    return (id >= 0) ? TIMER_ERR_SUCCESS : TIMER_ERR_NO_RESOURCE;
}

timer_error_e app_timer_stop(const char *name)
{
    if (name == NULL || name[0] == '\0') {
        return TIMER_ERR_INVALID_PARAM;
    }

    int id = find_timer_by_name(name);
    if (id >= 0) {
        my_soft_timer[id].state = TIMER_STATE_INACTIVE;
        return TIMER_ERR_SUCCESS;
    }
    return TIMER_ERR_NOT_FOUND;
}

bool app_timer_is_active(const char *name)
{
    int id = find_timer_by_name(name);
    return (id >= 0) ? (my_soft_timer[id].state == TIMER_STATE_ACTIVE) : false;
}

void app_timer_poll(void)
{
    uint32_t now = system_ticks;

    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (my_soft_timer[i].state != TIMER_STATE_PENDING)
            continue;

        // 执行回调
        if (my_soft_timer[i].callback) {
            my_soft_timer[i].callback(my_soft_timer[i].user_arg);
        }

        // 更新状态和 start_time
        if (my_soft_timer[i].repeat) {
            // 累积间隔，减少误差
            my_soft_timer[i].start_time += my_soft_timer[i].interval_ms;

            // 如果已经严重滞后（poll 很久没调用），也修正 start_time
            if ((int32_t)(now - my_soft_timer[i].start_time) >= (int32_t)my_soft_timer[i].interval_ms) {
                my_soft_timer[i].start_time = now;
            }

            my_soft_timer[i].state = TIMER_STATE_ACTIVE;
        } else {
            my_soft_timer[i].state = TIMER_STATE_INACTIVE;
        }
    }
}

// 查找空闲定时器
static int find_free_timer(void)
{
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (my_soft_timer[i].state == TIMER_STATE_INACTIVE) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 通过名称查找定时器
 * @param name 定时器名称
 * @return 定时器索引, -1表示未找到
 */
static int find_timer_by_name(const char *name)
{
    if (name == NULL || strlen(name) == 0) {
        return -1;
    }
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (my_soft_timer[i].state != TIMER_STATE_INACTIVE &&
            strcmp(my_soft_timer[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

uint32_t app_timer_get_ticks(void)
{
    return system_ticks;
}

void BTim3_IRQHandler(void)
{
    if (TRUE == Btim_IsActiveFlag(BTIM3, BTIM_IT_FLAG_UI)) {
        system_ticks++;

        for (uint8_t i = 0; i < MAX_SOFT_TIMERS; i++) {
            if (my_soft_timer[i].state != TIMER_STATE_ACTIVE)
                continue;

            uint32_t elapsed = system_ticks - my_soft_timer[i].start_time;
            if (elapsed >= my_soft_timer[i].interval_ms) {
                my_soft_timer[i].state = TIMER_STATE_PENDING;
            }
        }

        Btim_ClearFlag(BTIM3, BTIM_IT_CLR_UI);
    }
}