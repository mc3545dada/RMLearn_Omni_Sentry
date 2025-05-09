
#include "can.h"
#include "bsp_can.h"
#include "motor.h"
#include "Daemon.h"
#include "communication.h"
int a=0;

//CAN1和CAN2滤波器配置(使用相同滤波器配置）
void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan,uint32_t FilterFIFOAssignment,uint32_t FilterBank)
{
	//can1 &can2 use same filter config
	CAN_FilterTypeDef		CAN_FilterConfigStructure;

//	CAN_FilterConfigStructure.FilterNumber = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = FilterFIFOAssignment;//CAN_FilterFIFO0 or
	CAN_FilterConfigStructure.FilterBank = FilterBank;//can1(0-13)和can2(14-27)分别得到一半的filter
	CAN_FilterConfigStructure.FilterActivation = ENABLE;

	HAL_CAN_ConfigFilter(_hcan, &CAN_FilterConfigStructure);//使能滤波器配置



}

//HAL库的CAN接收完成回调函数，需要在此处理通过CAN总线接收到的数据
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{

        daemon_cntreset();
        CAN_RxHeaderTypeDef Rx1Message;
        uint8_t Data[8];


//	    Rx1Message.StdId = 0x200;  // 0x200 + CANID    此处自动获取，无需设置
        Rx1Message.ExtId = 0;  // CAN_ID_STD, ExtId is invalid
        Rx1Message.IDE = CAN_ID_STD;
        Rx1Message.RTR = CAN_RTR_DATA;
        Rx1Message.DLC = 0x08;

        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Rx1Message, Data);
        //哪个can总线 要读取的can接受fifo编号(关于滤波器） rx结构体地址  接收到的数据



        if (hcan->Instance == CAN1)
        {
            // 处理 CAN1 的消息
            get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//底盘电机，在motor.h中定义

        }
        else if (hcan->Instance == CAN2)
        {
            // 处理 CAN2 的消息

            if(Rx1Message.StdId ==0x205)
            {
                get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],
                                   Data[7]);//yaw轴电机
            }

            if(Rx1Message.StdId ==0x345)
            {
                get_tran_msg(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//收自定日id的消息

            }


        }








	/*#### add enable can it again to solve can receive only one ID problem!!!####**/
//	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//  __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);

    //只是这个地方不知道需不需要改，其他均扩展好了
}




//void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan)
//{
//
////    daemon_cntreset();
//    CAN_RxHeaderTypeDef Rx2Message;
//    uint8_t Data[8];
//    uint8_t MotorID;
//
////	    Rx1Message.StdId = 0x200;  // 0x200 + CANID    此处自动获取，无需设置
//    Rx2Message.ExtId = 0;  // CAN_ID_STD, ExtId is invalid
//    Rx2Message.IDE = CAN_ID_STD;
//    Rx2Message.RTR = CAN_RTR_DATA;
//    Rx2Message.DLC = 0x08;
//
//    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &Rx2Message, Data);
//    //哪个can总线 要读取的can接受fifo编号(关于滤波器） rx结构体地址  接收到的数据
//
//    switch (Rx2Message.StdId) {
//        case CAN_Moto1_ID:
//            MotorID = 0;
//            break;
//        case CAN_Moto2_ID:
//            MotorID = 1;
//            break;
//        case CAN_Moto3_ID:
//            MotorID = 2;
//            break;
//        case CAN_Moto4_ID:
//            MotorID = 3;
//            break;
//        case CAN_Moto5_ID:
//            MotorID = 4;
//            break;
//        case CAN_Moto6_ID:
//            MotorID = 5;
//            break;
//        case CAN_Moto7_ID:
//            MotorID = 6;
//            break;
//        case CAN_Moto8_ID:
//            MotorID = 7;
//            break;
//
//    }
//    get_chassis_mesure(MotorID, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],
//                       Data[7]);//在motor.h中定义
//
//    /*#### add enable can it again to solve can receive only one ID problem!!!####**/
////    __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//    __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
//
//    //只是这个地方不知道需不需要改，其他均扩展好了
//}





void can_sendtxmessage(CAN_HandleTypeDef* hcan,uint32_t stdid,uint8_t D0,uint8_t D1,uint8_t D2,uint8_t D3,uint8_t D4,uint8_t D5,uint8_t D6,uint8_t D7)//在motor.h中调用
{
	/////can发送电机数据__________________________________________________________
	CAN_TxHeaderTypeDef CanTx;
	uint8_t Data[8];
	uint32_t Mailbox=0;

	CanTx.StdId = stdid;  // 0x200
	CanTx.ExtId = 0;  // CAN_ID_STD, ExtId is invalid
	CanTx.IDE = CAN_ID_STD;
	CanTx.RTR = CAN_RTR_DATA;
	CanTx.DLC = 0x08;
	CanTx.TransmitGlobalTime = DISABLE;

	Data[0] = D0;
	Data[1] = D1;
	Data[2] = D2;
	Data[3] = D3;
	Data[4] = D4;
	Data[5] = D5;
	Data[6] = D6;
	Data[7] = D7;
    if (hcan->Instance == CAN2)
    {
        HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        a=0;
//        a=1;
    }

    if (hcan->Instance == CAN1)
    {
        HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        a=0;
//        a=1;
    }


}	//哪个can总线   结构体帧头   要写入的数据数组 返回发送邮箱的编号

//void set_gimbal_current(CAN_HandleTypeDef* hcan s16 gb1,s16 gb2,s16 gb3,s16,gb4)
//后续云台代码