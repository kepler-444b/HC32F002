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

#define UART_FRAME_TIMEOUT_MS 20

#define RD_SET                GPIO_PC04_SET()   // 拉高 485 使能脚
#define RD_RESET              GPIO_PC04_RESET() // 拉低 485 使能脚

static volatile uint32_t last_rx_ticks = 0;

static uart_state_t rx_state   = WAIT_HEAD1;
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

    RD_RESET;
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

    RD_SET; // 进入发送模式
    LPUART_Transmit(LPUART1, data, len, 300);
    while (!LPUART_GetFlag(LPUART1, LPUART_FLAG_TC)) {
        ;
    }
    delay100us(10); // 短暂延时,防止丢字节
    RD_RESET;
}

void LPUART1_IRQHandler(void)
{
    uint8_t data; // 当前中断接收到的1字节数据

    if (LPUART_GetFlag(LPUART1, LPUART_FLAG_RC)) {

        if (LPUART_GetFlag(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE)) { // 处理错误中断
            LPUART_ClearIrq(LPUART1, LPUART_FLAG_PE | LPUART_FLAG_FE);
            rx_state       = WAIT_HEAD1;
            rx1_buf.length = 0;
            return;
        }

        LPUART_ReceiveIT(LPUART1, &data);         // 读取1字节数据,触发接收中断
        LPUART_ClearIrq(LPUART1, LPUART_FLAG_RC); // 清除接收中断标志位

        switch (rx_state) {
            case WAIT_HEAD1: // 等待 0xFF
                if (data == FRAME_HEAD_1) {
                    rx_state                         = WAIT_HEAD2;
                    rx1_buf.length                   = 0;
                    rx1_buf.buffer[rx1_buf.length++] = data;
                }
                break;

            case WAIT_HEAD2: // 等待 0xAA
                if (data == FRAME_HEAD_2) {
                    rx_state                         = RECEIVING;
                    rx1_buf.buffer[rx1_buf.length++] = data;
                } else {
                    rx_state = WAIT_HEAD1; // 重新同步
                }
                break;

            case RECEIVING: // 接收数据

                if (rx1_buf.length < UART1_RECV_SIZE) { // 防止缓冲区溢出
                    rx1_buf.buffer[rx1_buf.length++] = data;
                } else {
                    rx_state       = WAIT_HEAD1;
                    rx1_buf.length = 0;
                    break;
                }
                if (rx1_buf.length >= 2) { // 检测帧尾
                    if (rx1_buf.buffer[rx1_buf.length - 2] == FRAME_TAIL_1 && rx1_buf.buffer[rx1_buf.length - 1] == FRAME_TAIL_2) {

                        rx1_buf.is_completed = true; // 标记完成,交给主循环处理
                        rx_state             = WAIT_HEAD1;
                    }
                }
                break;
        }
    }
}

void bsp_usart_has_data(void)
{
    if (rx1_buf.is_completed) {
        if (rx1_callback) {
            rx1_callback((usart1_rx_buf_t *)&rx1_buf);
        }
        rx1_buf.length       = 0;
        rx1_buf.is_completed = false;
    }
}

int fputc(int ch, FILE *f)
{
#if defined APP_DEBUG
    SEGGER_RTT_PutChar(0, ch);
#endif
    return ch;
}