#ifndef __DBUSCONTROL_H
#define __DBUSCONTROL_H

#define DBUS_MAX_LEN     (50)
#define DBUS_BUFLEN      (18)
#define DBUS_HUART       huart3
#include "stm32f4xx_hal_uart.h"
#include <stdint.h>

typedef struct
{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    int16_t roll;
    uint8_t sw1;
    uint8_t sw2;

    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;

        uint8_t l;
        uint8_t r;
    } mouse;
    /* keyboard key information */
    union {
        uint16_t key_code;
        struct
        {
            uint16_t W : 1;
            uint16_t S : 1;
            uint16_t A : 1;
            uint16_t D : 1;
            uint16_t SHIFT : 1;
            uint16_t CTRL : 1;
            uint16_t Q : 1;
            uint16_t E : 1;
            uint16_t R : 1;
            uint16_t F : 1;
            uint16_t G : 1;
            uint16_t Z : 1;
            uint16_t X : 1;
            uint16_t C : 1;
            uint16_t V : 1;
            uint16_t B : 1;
        } bit;
    } kb;

} rc_info_t;

#define rc_Init   \
{                 \
0,            \
0,            \
0,            \
0,            \
0,            \
0,            \
0,            \
}

void dbus_uart_init(void);
static int uart_receive_dma_no_it(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size);
void rc_callback_handler(rc_info_t *rc, uint8_t *buff);
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream);
static void uart_rx_idle_callback(UART_HandleTypeDef* huart);
void uart_receive_handler(UART_HandleTypeDef *huart);

#endif