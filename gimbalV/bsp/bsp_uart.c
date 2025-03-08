//
// Created by 3545 on 2024/11/15.
//
#include "bsp_uart.h"
#include "usart.h"
#include "main.h"
char rx_data[64];


//本底盘代码用到了三个uart接口
//    c版上串口名        实际串口名字         用途                收/发                类型              位置
//    DBUS              usart3        遥控器数据接受            收+发             DMA+空闲中断       DBUScontrol.c中
//    usart2            usart1      调试用(返回电机数据等)      发（目前是）            DMA           chassis.c中
//    usart6            usart2        预留的（接受视觉等）
//
//
//


//预留视觉等串口接口
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//    if(huart == &huart6)//检测串口是所需要的串口
//    {
//        HAL_UART_Transmit_DMA(huart, (uint8_t *)&rx_data,Size);//自收自发，调试用
//
//          //此处为需要编写的代码
//
//        HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)&rx_data,sizeof(rx_data));
//    }
//}
//
// 此外，还需要在主函数中，osstart前，初始第一次dma接收串口数据，才能不断触发此回调函数