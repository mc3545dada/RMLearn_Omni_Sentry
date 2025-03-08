//
// Created by 3545 on 2024/11/14.
//

#ifndef _MOTOR_H
#define _MOTOR_H



void set_chassis_current(CAN_HandleTypeDef* hcan,uint32_t stdid,s16 iq1, s16 iq2, s16 iq3, s16 iq4);
void get_chassis_mesure(uint32_t stdid,uint8_t D0,uint8_t D1,uint8_t D2,uint8_t D3,uint8_t D4,uint8_t D5,uint8_t D6,uint8_t D7);



#endif
