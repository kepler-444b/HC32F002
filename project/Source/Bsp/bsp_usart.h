#ifndef _USART_H_
#define _USART_H_

#include <stdint.h>
#include "lpuart.h"

// #define APP_DEBUG // 此宏用来管理整个工程的 debug 信息

#if defined APP_DEBUG
#define APP_PRINTF(...) printf(__VA_ARGS__)
#define APP_PRINTF_BUF(name, buf, len)                        \
    do {                                                      \
        APP_PRINTF("%s: ", (name));                           \
        for (size_t i = 0; i < (len); i++) {                  \
            APP_PRINTF("%02X ", ((const uint8_t *)(buf))[i]); \
        }                                                     \
        APP_PRINTF("\n");                                     \
    } while (0)
#define APP_ERROR(fmt, ...) \
    APP_PRINTF("[#%s#] \"" fmt "\" ERROR!\n", __func__, ##__VA_ARGS__)

#else

#define APP_PRINTF(...)
#define APP_PRINTF_BUF(name, buf, len)
#define APP_ERROR(fmt, ...)
#endif

#define UART1_RECV_SIZE 256

typedef struct
{
    uint8_t buffer[UART1_RECV_SIZE];
    uint16_t length;
} usart1_rx_buf_t;

typedef void (*usart_rx1_callback_t)(usart1_rx_buf_t *);
void bsp_usart1_rx_callback(usart_rx1_callback_t callback);

void bsp_usart_init(uint32_t baudrate);
void bsp_usart_has_data(void);
void bsp_usart_tx(uint8_t *data, uint16_t len);

#endif