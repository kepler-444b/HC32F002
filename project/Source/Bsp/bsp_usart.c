#include "bsp_usart.h"
#include "lpm.h"
#include "gpio.h"
#include "stdbool.h"
#include "gtim.h"
#include "../Source/App/app_timer.h"
#include "../Source/Bsp/bsp_pcb.h"
#if defined APP_DEBUG
#include "../Source/Rtt/SEGGER_RTT.h"
#endif

#define UART_FRAME_TIMEOUT_MS 10

#define RD_SET                GPIO_PC04_SET()   // 拉高 485 使能脚
#define RD_RESET              GPIO_PC04_RESET() // 拉低 485 使能脚

static volatile bool has_data          = false;
static volatile uint32_t last_rx_ticks = 0;

static usart1_rx_buf_t rx1_buf = {0};

static usart_rx1_callback_t rx1_callback = NULL;
void bsp_usart1_rx_callback(usart_rx1_callback_t callback)
{
    rx1_callback = callback;
}

static void bsp_cfg_usart_init(uint32_t baudrate);
void bsp_usart_init(uint32_t baudrate)
{

    bsp_pcb_RD_init();    // 初始化 485 使能引脚
    bsp_pcb_usart_init(); // 初始化 usart rx/tx引脚

    bsp_cfg_usart_init(baudrate); // 初始化 usart 配置信息
}

static void bsp_cfg_usart_init(uint32_t baudrate)
{
    stc_lpuart_init_t stcInit = {0};
    SYSCTRL_PeriphClkEnable(SYSCTRL_PERICLK_LPUART1);
    stcInit.u32StopBits             = LPUART_STOPBITS_1;
    stcInit.u32Parity               = LPUART_PARITY_NONE;
    stcInit.u32FrameLength          = LPUART_FRAME_LEN_8BITS;
    stcInit.u32TransMode            = LPUART_MODE_TX_RX;
    stcInit.stcBaudRate.u32SclkSrc  = LPUART_CLKSRC_PCLK;
    stcInit.stcBaudRate.u32Sclk     = SYSCTRL_GetPCLK();
    stcInit.stcBaudRate.u32BaudRate = baudrate;
    LPUART_Init(LPUART1, &stcInit);

    LPUART_ClearIrq(LPUART1, LPUART_FLAG_ALL);
    LPUART_EnableIrq(LPUART1, LPUART_INT_RC);
    EnableNvic(LPUART1_IRQn, IrqLevel1, TRUE);
}

void bsp_usart_tx(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return;
    }

    LPUART_DisableIrq(LPUART1, LPUART_INT_RC); // 禁用接收中断
    LPUART_ClearIrq(LPUART1, LPUART_FLAG_RC);  // 清空中断标志位

    RD_SET;
    LPUART_Transmit(LPUART1, data, len, 300);
    while (!LPUART_GetFlag(LPUART1, LPUART_FLAG_TC)) {
        ;
    }
    LPUART_EnableIrq(LPUART1, LPUART_INT_RC); // 启用接收中断
    rx1_buf.length = 0;
    delay1ms(1); // 短暂延时,防止丢字节
    RD_RESET;
}

void LPUART1_IRQHandler(void)
{
    if (LPUART_GetFlag(LPUART1, LPUART_FLAG_RC)) {

        if (LPUART_GetFlag(LPUART1, LPUART_FLAG_RC)) { // 接收完成

            if (LPUART_GetFlag(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE)) { // 处理错误
                LPUART_ClearIrq(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE);
                rx1_buf.length = 0;
                return;
            }

            if (rx1_buf.length >= UART1_RECV_SIZE) { // 溢出缓冲区,丢弃
                rx1_buf.length = 0;
                return;
            }

            if (rx1_buf.length < UART1_RECV_SIZE) {
                LPUART_ReceiveIT(LPUART1, &rx1_buf.buffer[rx1_buf.length]);
                rx1_buf.length++;
                last_rx_ticks = app_timer_get_ticks(); // 更新最后接收时间
            }
            LPUART_ClearIrq(LPUART1, LPUART_FLAG_RC);
        }
    }
}

void bsp_usart_has_data(void)
{
    if (rx1_buf.length == 0) return;

    if ((app_timer_get_ticks() - last_rx_ticks) >= UART_FRAME_TIMEOUT_MS) {
        if (rx1_callback) {
            rx1_callback(&rx1_buf);
        }
        rx1_buf.length = 0; // 清空缓冲
    }
}

int fputc(int ch, FILE *f)
{
#if defined APP_DEBUG
    SEGGER_RTT_PutChar(0, ch);
#endif
    return ch;
}