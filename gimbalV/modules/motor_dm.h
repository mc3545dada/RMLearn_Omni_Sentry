//
// Created by guan on 2024/11/21.
//

#ifndef MOTOR_DM_H
#define MOTOR_DM_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define MOTOR_DM_ANGLE_MAX  0.430f

/**
 * @brief 达妙电机状态
 *
 */
enum Enum_Motor_DM_Status
{
    Motor_DM_Status_DISABLE = 0,
    Motor_DM_Status_ENABLE,
};

/**
 * @brief 达妙电机控制状态, 传统模式有效
 *
 */
enum Enum_Motor_DM_Control_Status_Normal
{
    Motor_DM_Control_Status_DISABLE = 0x0,
    Motor_DM_Control_Status_ENABLE,
    Motor_DM_Control_Status_OVERVOLTAGE = 0x8,
    Motor_DM_Control_Status_UNDERVOLTAGE,
    Motor_DM_Control_Status_OVERCURRENT,
    Motor_DM_Control_Status_MOS_OVERTEMPERATURE,
    Motor_DM_Control_Status_ROTOR_OVERTEMPERATURE,
    Motor_DM_Control_Status_LOSE_CONNECTION,
    Motor_DM_Control_Status_MOS_OVERLOAD,
};

/**
 * @brief 达妙电机控制方式
 *
 */
enum Enum_Motor_DM_Control_Method
{
    Motor_DM_Control_Method_NORMAL_MIT = 0,
    Motor_DM_Control_Method_NORMAL_ANGLE_OMEGA,
    Motor_DM_Control_Method_NORMAL_OMEGA,
    Motor_DM_Control_Method_NORMAL_EMIT,
    Motor_DM_Control_Method_1_TO_4_CURRENT,
    Motor_DM_Control_Method_1_TO_4_OMEGA,
    Motor_DM_Control_Method_1_TO_4_ANGLE,
};

/**
 * @brief 达妙电机传统模式源数据
 *
 */
typedef struct
{
    uint8_t CAN_ID;
    uint8_t Control_Status_Enum;
    uint8_t Angle_15_8;
    uint8_t Angle_7_0;
    uint8_t Omega_11_4;
    uint8_t Omega_3_0_Torque_11_8;
    uint8_t Torque_7_0;
    uint8_t MOS_Temperature;
    uint8_t Rotor_Temperature;
}Struct_Motor_DM_CAN_Rx_Data_Normal;

/**
 * @brief 达妙电机常规源数据, 位置速度控制报文
 *
 */
typedef struct
{
    float Control_Angle;
    float Control_Omega;
}Struct_Motor_DM_CAN_Tx_Data_Normal_Angle_Omega;

/**
 * @brief 达妙电机经过处理的数据, 传统模式有效
 *
 */
typedef struct
{
    enum Enum_Motor_DM_Control_Status_Normal Control_Status;
    float Now_Angle;
    float Now_Omega;
    float Now_Torque;
    float Now_MOS_Temperature;
    float Now_Rotor_Temperature;
    uint32_t Pre_Encoder;
    int32_t Total_Encoder;
    int32_t Total_Round;
}Struct_Motor_DM_Rx_Data_Normal;

/**
 * @brief Reusable, 达妙电机, 传统模式
 * 没有零点, 可在上位机调零点
 * 初始化的角度, 角速度, 扭矩, 电流等参数是J4310电机默认值
 *
 */
typedef struct
{
    // 初始化相关变量
    Struct_Motor_DM_Rx_Data_Normal motor_dm_rx_data;
    // 常量

    // 绑定的CAN
    CAN_HandleTypeDef *can_handle;
    // 收数据绑定的CAN ID, 与上位机驱动参数Master_ID保持一致
    uint16_t CAN_Rx_ID;
    // 发数据绑定的CAN ID, 是上位机驱动参数CAN_ID加上控制模式的偏移量
    uint16_t CAN_Tx_ID;
    // 最大位置, 与上位机控制幅值PMAX保持一致
    float Angle_Max;
    // 最大速度, 与上位机控制幅值VMAX保持一致
    float Omega_Max;
    // 最大扭矩, 与上位机控制幅值TMAX保持一致
    float Torque_Max;
    // 最大电流, 与上位机串口中上电打印电流保持一致
    float Current_Max;

    // 内部变量

    // 当前时刻的电机接收flag
    uint32_t Flag;
    // 前一时刻的电机接收flag
    uint32_t Pre_Flag;

    // 发送缓冲区
    uint8_t Tx_Data[8];

    // 读变量

    // 电机状态
    enum Enum_Motor_DM_Status Motor_DM_Status;
    // 电机对外接口信息
    Struct_Motor_DM_Rx_Data_Normal Rx_Data;

    // 写变量

    // 读写变量

    // 电机控制方式
    enum Enum_Motor_DM_Control_Method Motor_DM_Control_Method;

    // 角度, rad, 目标角度
    float Control_Angle;
    // 角速度, rad/s, MIT模式和速度模式是目标角速度, 其余模式是限幅
    float Control_Omega;
    // 扭矩, Nm, MIT模式是目标扭矩, EMIT模式无效, 其余模式是限幅
    float Control_Torquef;
    // 电流, A, EMIT模式是限幅, 其余模式无效
    float Control_Current;
    // K_P, 0~500, MIT模式有效
    float K_P;
    // K_D, 0~5, MIT模式有效
    float K_Df;

}MOTOR_DM;

extern MOTOR_DM motor_dm;
extern Struct_Motor_DM_CAN_Rx_Data_Normal motor_dm_data;
extern Struct_Motor_DM_CAN_Tx_Data_Normal_Angle_Omega motor_dm_angle_omega;

/**
 * @brief 电机初始化
 *
 * @param hcan 绑定的CAN总线
 * @param __CAN_Rx_ID 收数据绑定的CAN ID, 与上位机驱动参数Master_ID保持一致, 传统模式有效
 * @param __CAN_Tx_ID 发数据绑定的CAN ID, 是上位机驱动参数CAN_ID加上控制模式的偏移量, 传统模式有效
 * @param __Motor_DM_Control_Method 电机控制方式
 * @param __Angle_Max 最大位置, 与上位机控制幅值PMAX保持一致, 传统模式有效
 * @param __Omega_Max 最大速度, 与上位机控制幅值VMAX保持一致, 传统模式有效
 * @param __Torque_Max 最大扭矩, 与上位机控制幅值TMAX保持一致, 传统模式有效
 */
void Motor_DM_Init(CAN_HandleTypeDef *hcan, uint8_t __CAN_Rx_ID, uint8_t __CAN_Tx_ID, enum Enum_Motor_DM_Control_Method __Motor_DM_Control_Method, float __Angle_Max, float __Omega_Max, float __Torque_Max, float __Current_Max);

/**
 * @brief 发送清除错误信息
 *
 */
void Motor_DM_Send_Clear_Error();

/**
 * @brief 发送使能电机
 *
 */
void Motor_DM_Send_Enter();

/**
 * @brief 发送失能电机
 *
 */
void Motor_DM_Send_Exit();

/**
 * @brief 发送保存当前位置为零点
 *
 */
void Motor_DM_Send_Save_Zero();

/**
 * @brief 数据处理过程
 *
 */
void Motor_DM_Normal_Data_Process();

/**
 * @brief 获取角度, rad, 目标角度
 *
 * @return float 角度, rad, 目标角度
 */
void Motor_DM_Get_Control_Angle(float Control_Angle);

/**
 * @brief 获取角速度, rad/s, MIT模式和速度模式是目标角速度, 其余模式是限幅
 *
 * @return float 角速度, rad/s, MIT模式和速度模式是目标角速度, 其余模式是限幅
 */
void Motor_DM_Get_Control_Omega(float Control_Omega);

void cmd_motor_dm();
void get_motor_dm(uint32_t stdid, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);

#endif //MOTOR_DM_H
