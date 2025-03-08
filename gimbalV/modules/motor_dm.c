//
// Created by guan on 2024/11/21.
//

#include "motor_dm.h"

#include "can.h"
#include "chassis.h"
#include "bsp_can.h"

// 清除电机错误信息, 传统模式有效
uint8_t DM_Motor_CAN_Message_Clear_Error[8] = {0xff,
                                               0xff,
                                               0xff,
                                               0xff,
                                               0xff,
                                               0xff,
                                               0xff,
                                               0xfb};
// 使能电机, 传统模式有效
uint8_t DM_Motor_CAN_Message_Enter[8] = {0xff,
                                         0xff,
                                         0xff,
                                         0xff,
                                         0xff,
                                         0xff,
                                         0xff,
                                         0xfc};
// 失能电机, 传统模式有效
uint8_t DM_Motor_CAN_Message_Exit[8] = {0xff,
                                        0xff,
                                        0xff,
                                        0xff,
                                        0xff,
                                        0xff,
                                        0xff,
                                        0xfd};
// 保存当前电机位置为零点, 传统模式有效
uint8_t DM_Motor_CAN_Message_Save_Zero[8] = {0xff,
                                             0xff,
                                             0xff,
                                             0xff,
                                             0xff,
                                             0xff,
                                             0xff,
                                             0xfe};

MOTOR_DM motor_dm;
Struct_Motor_DM_CAN_Rx_Data_Normal motor_dm_data;
Struct_Motor_DM_CAN_Tx_Data_Normal_Angle_Omega motor_dm_angle_omega;

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
void Motor_DM_Init(CAN_HandleTypeDef *hcan, uint8_t __CAN_Rx_ID, uint8_t __CAN_Tx_ID, enum Enum_Motor_DM_Control_Method __Motor_DM_Control_Method, float __Angle_Max, float __Omega_Max, float __Torque_Max, float __Current_Max)
{
    motor_dm.can_handle = hcan;

    motor_dm.CAN_Rx_ID = __CAN_Rx_ID;
    switch (__Motor_DM_Control_Method)
    {
    case (Motor_DM_Control_Method_NORMAL_MIT):
        {
            motor_dm.CAN_Tx_ID = __CAN_Tx_ID;
            break;
        }
    case (Motor_DM_Control_Method_NORMAL_ANGLE_OMEGA):
        {
            motor_dm.CAN_Tx_ID = __CAN_Tx_ID + 0x100;
            break;
        }
    case (Motor_DM_Control_Method_NORMAL_OMEGA):
        {
            motor_dm.CAN_Tx_ID = __CAN_Tx_ID + 0x200;
            break;
        }
    case (Motor_DM_Control_Method_NORMAL_EMIT):
        {
            motor_dm.CAN_Tx_ID = __CAN_Tx_ID + 0x300;
            break;
        }
    default:
        break;
    }
    motor_dm.Motor_DM_Control_Method = __Motor_DM_Control_Method;
    motor_dm.Motor_DM_Status = Motor_DM_Status_DISABLE;
    motor_dm.Angle_Max = __Angle_Max;
    motor_dm.Omega_Max = __Omega_Max;
    motor_dm.Torque_Max = __Torque_Max;
    motor_dm.Current_Max = __Current_Max;
    motor_dm_angle_omega.Control_Angle = 0.0f;
    motor_dm_angle_omega.Control_Omega = 3.14159265358979f;
}

void Motor_dm_send(uint32_t ID, uint8_t data[8])
{
    CAN_TxHeaderTypeDef motor_tx_message;
    uint32_t send_mail_box;
    motor_tx_message.StdId = ID;
    motor_tx_message.ExtId = 0;
    motor_tx_message.DLC = 0x08;
    motor_tx_message.IDE = CAN_ID_STD;
    motor_tx_message.RTR = CAN_RTR_DATA;
    //上次未写
    motor_tx_message.TransmitGlobalTime = DISABLE;
    //上次未写
    HAL_CAN_AddTxMessage(motor_dm.can_handle, &motor_tx_message, data, &send_mail_box);
}

/**
 * @brief 发送清除错误信息
 *
 */
void Motor_DM_Send_Clear_Error()
{
    Motor_dm_send(motor_dm.CAN_Tx_ID, DM_Motor_CAN_Message_Clear_Error);
}

/**
 * @brief 发送使能电机
 *
 */
void Motor_DM_Send_Enter()
{
    Motor_dm_send(motor_dm.CAN_Tx_ID, DM_Motor_CAN_Message_Enter);
}

/**
 * @brief 发送失能电机
 *
 */
void Motor_DM_Send_Exit()
{
    Motor_dm_send(motor_dm.CAN_Tx_ID, DM_Motor_CAN_Message_Exit);
}

/**
 * @brief 发送保存当前位置为零点
 *
 */
void Motor_DM_Send_Save_Zero()
{
    Motor_dm_send(motor_dm.CAN_Tx_ID, DM_Motor_CAN_Message_Save_Zero);
}

/**
 * @brief 将整型映射到浮点数
 *
 * @param x 整型
 * @param Int_Min 整型最小值
 * @param Int_Max 整型最大值
 * @param Float_Min 浮点数最小值
 * @param Float_Max 浮点数最大值
 * @return float 浮点数
 */
float Math_Int_To_Float(int32_t x, int32_t Int_Min, int32_t Int_Max, float Float_Min, float Float_Max)
{
    float tmp = (float) (x - Int_Min) / (float) (Int_Max - Int_Min);
    float out = tmp * (Float_Max - Float_Min) + Float_Min;
    return (out);
}

/**
 * @brief 数据处理过程
 *
 */
void Motor_DM_Normal_Data_Process()
{
    // 数据处理过程
    int32_t delta_encoder;
    uint16_t tmp_encoder, tmp_omega, tmp_torque;

    // 处理大小端
    tmp_encoder = (motor_dm_data.Angle_15_8 << 8) | (motor_dm_data.Angle_7_0);
    tmp_omega = (motor_dm_data.Omega_11_4 << 4) | (motor_dm_data.Omega_3_0_Torque_11_8 >> 4);
    tmp_torque = ((motor_dm_data.Omega_3_0_Torque_11_8 & 0x0f) << 8) | (motor_dm_data.Torque_7_0);


    // 计算圈数与总角度值
    delta_encoder = tmp_encoder - motor_dm.motor_dm_rx_data.Pre_Encoder;
    if (delta_encoder < -(1 << 15))
    {
        // 正方向转过了一圈
        motor_dm.motor_dm_rx_data.Total_Round++;
    }
    else if (delta_encoder > (1 << 15))
    {
        // 反方向转过了一圈
        motor_dm.motor_dm_rx_data.Total_Round--;
    }
    motor_dm.motor_dm_rx_data.Total_Encoder = motor_dm.motor_dm_rx_data.Total_Round * (1 << 16) + tmp_encoder - ((1 << 15) - 1);

    // 计算电机本身信息
    motor_dm.motor_dm_rx_data.Now_Angle = (float)(motor_dm.motor_dm_rx_data.Total_Encoder) / (float)((1 << 16) - 1) * motor_dm.Angle_Max * 2.0f;
    motor_dm.motor_dm_rx_data.Now_Omega = Math_Int_To_Float(tmp_omega, 0x7ff, (1 << 12) - 1, 0, motor_dm.Omega_Max);
    motor_dm.motor_dm_rx_data.Now_Torque = Math_Int_To_Float(tmp_torque, 0x7ff, (1 << 12) - 1, 0, motor_dm.Torque_Max);
    motor_dm.motor_dm_rx_data.Now_MOS_Temperature = motor_dm_data.MOS_Temperature;
    motor_dm.motor_dm_rx_data.Now_Rotor_Temperature = motor_dm_data.Rotor_Temperature;

    // 存储预备信息
    motor_dm.motor_dm_rx_data.Pre_Encoder = tmp_encoder;
}

/**
 * @brief 获取角度, rad, 目标角度
 *
 * @return float 角度, rad, 目标角度
 */
void Motor_DM_Get_Control_Angle(float Control_Angle)
{

    motor_dm_angle_omega.Control_Angle = Control_Angle;
}

/**
 * @brief 获取角速度, rad/s, MIT模式和速度模式是目标角速度, 其余模式是限幅
 *
 * @return float 角速度, rad/s, MIT模式和速度模式是目标角速度, 其余模式是限幅
 */
void Motor_DM_Get_Control_Omega(float Control_Omega)
{
    motor_dm_angle_omega.Control_Omega = Control_Omega;
}








void cmd_motor_dm()
{

    uint8_t *pbuf, *vbuf;
    uint8_t data[8];

    pbuf=(uint8_t*)&motor_dm_angle_omega.Control_Angle;///
    vbuf=(uint8_t*)&motor_dm_angle_omega.Control_Omega;///

//    CAN_TxHeaderTypeDef trans_tx_message;
//    uint32_t send_mail_box;

//    trans_tx_message.StdId = 0x102;//
//    trans_tx_message.ExtId = 0;
//    trans_tx_message.DLC = 0x08;
//    trans_tx_message.IDE = CAN_ID_STD;
//    trans_tx_message.RTR = CAN_RTR_DATA;
//    trans_tx_message.TransmitGlobalTime = DISABLE;

//    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
//        ; // 等待邮箱空闲

    data[0] = *pbuf;
    data[1] = *(pbuf+1);
    data[2] = *(pbuf+2);
    data[3] = *(pbuf+3);

    data[4] = *vbuf;
    data[5] = *(vbuf+1);
    data[6] = *(vbuf+2);
    data[7] = *(vbuf+3);


//    HAL_CAN_AddTxMessage(&hcan1, &trans_tx_message, data, &send_mail_box);

    can_senddm(&hcan1,0x102,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);

}

void get_motor_dm(uint32_t stdid, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7)
{
    motor_dm_data.CAN_ID = D0;
    motor_dm_data.Control_Status_Enum = D0 >> 4;
    motor_dm_data.Angle_15_8 = D1;
    motor_dm_data.Angle_7_0 = D2;
    motor_dm_data.Omega_11_4 = D3;
    motor_dm_data.Omega_3_0_Torque_11_8 = D4;
    motor_dm_data.Torque_7_0 = D5;
    motor_dm_data.MOS_Temperature = D6;
    motor_dm_data.Rotor_Temperature = D7;

    Motor_DM_Normal_Data_Process();



}

