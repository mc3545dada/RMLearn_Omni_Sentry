//
// Created by 3545 on 2024/11/17.
//
#include "bsp_can.h"
#include "communication.h"

int16_t tran_msg1;
int16_t tran_msg2;
int16_t tran_msg3;
int16_t tran_msg4;

void get_tran_msg(uint32_t stdid, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7)
{
    if(stdid == 0x345)
    {
        tran_msg1 = (D0 << 8 | D1);
        tran_msg2 = (D2 << 8 | D3);
        tran_msg3 = (D4 << 8 | D5);
        tran_msg4 = (D6 << 8 | D7);
    }

}//需要使用此函数时，仅需要在chassis调用相应的变量即可



void set_tran_msg(CAN_HandleTypeDef* hcan, uint32_t stdid,s16 iq1, s16 iq2, s16 iq3, s16 iq4)
{
    uint8_t D0 = 0, D1 = 0, D2 = 0, D3 = 0, D4 = 0, D5 = 0, D6 = 0, D7 = 0;

    D0 = (iq1 >> 8);
    D1 = iq1;
    D2 = (iq2 >> 8);
    D3 = iq2;
    D4 = iq3 >> 8;
    D5 = iq3;
    D6 = iq4 >> 8;
    D7 = iq4;


    can_sendtxmessage(hcan,stdid,D0,D1,D2,D3,D4,D5,D6,D7);

}