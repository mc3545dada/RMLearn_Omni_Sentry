//
// Created by 3545 on 2024/11/14.
//
#include "can.h"
#include "bsp_can.h"
moto_measure_t moto_chassis[8] = {0};
void get_chassis_mesure(uint32_t stdid,uint8_t D0,uint8_t D1,uint8_t D2,uint8_t D3,uint8_t D4,uint8_t D5,uint8_t D6,uint8_t D7)
{
    uint8_t MotorID;
    switch (stdid)
    {
        case CAN_Moto1_ID:MotorID = 0;break;
        case CAN_Moto2_ID:MotorID = 1;break;
        case CAN_Moto3_ID:MotorID = 2;break;
        case CAN_Moto4_ID:MotorID = 3;break;
        case CAN_Moto5_ID:MotorID = 4;break;
        case CAN_Moto6_ID:MotorID = 5;break;
        case CAN_Moto7_ID:MotorID = 6;break;
        case CAN_Moto8_ID:MotorID = 7;break;

    }

    moto_chassis[MotorID].last_angle = moto_chassis[MotorID].angle;
    moto_chassis[MotorID].angle = (uint16_t)(D0<<8 | D1) ;
    moto_chassis[MotorID].speed_rpm  = (int16_t)(D2<<8 | D3);
    moto_chassis[MotorID].real_current = (D4<<8 | D5)*5.f/16384.f;
    moto_chassis[MotorID].hall = D6;
    if(moto_chassis[MotorID].angle - moto_chassis[MotorID].last_angle > 4096)
        moto_chassis[MotorID].round_cnt --;
    else if (moto_chassis[MotorID].angle - moto_chassis[MotorID].last_angle < -4096)
        moto_chassis[MotorID].round_cnt ++;
    moto_chassis[MotorID].total_angle = moto_chassis[MotorID].round_cnt * 8192 + moto_chassis[MotorID].angle - moto_chassis[MotorID].offset_angle;



}


void set_chassis_current(CAN_HandleTypeDef* hcan, uint32_t stdid,s16 iq1, s16 iq2, s16 iq3, s16 iq4)//发送底盘数据至bspcan发送文件
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

