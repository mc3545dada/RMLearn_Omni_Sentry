//
// Created by 3545 on 2024/11/17.
//

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H


void get_tran_msg(uint32_t stdid, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);
void set_tran_msg(CAN_HandleTypeDef* hcan, uint32_t stdid,s16 iq1, s16 iq2, s16 iq3, s16 iq4);

#endif
