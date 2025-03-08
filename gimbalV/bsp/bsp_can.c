
#include "can.h"
#include "bsp_can.h"
#include "motor.h"
#include "Daemon.h"
#include "communication.h"
#include "motor_dm.h"

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
            if(Rx1Message.StdId ==0x201)
            {
                get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//底盘电机，在motor.h中定义
            }
            if(Rx1Message.StdId ==0x204)
            {
                get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//底盘电机，在motor.h中定义
            }
            if(Rx1Message.StdId ==0x207)
            {
                get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//底盘电机，在motor.h中定义
            }

            if(Rx1Message.StdId==0x01)
            {
                get_motor_dm(0x01,Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);

            }


        }
        else if (hcan->Instance == CAN2)
        {
            // 处理 CAN2 的消息

//            if(Rx1Message.StdId ==0x205)
//            {
//                get_chassis_mesure(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],
//                                   Data[7]);//yaw轴电机
//            }

            if(Rx1Message.StdId ==0x345)
            {
                get_tran_msg(Rx1Message.StdId, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6],Data[7]);//收自定日id的消息

            }


        }

//    __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

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
//    if (hcan->Instance == CAN2)
//    {
//        HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        a=0;
//        a=1;
//    }

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
        ; // 等待邮箱空闲

    if (hcan->Instance == CAN1)
    {
        if(stdid==0x200)
        {
            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
        }

        if(stdid==0x1ff)
        {
            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
        }
//        if(stdid==0x102)
//        {
//            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        }


//        a=0;
//        a=1;
    }


}	//哪个can总线   结构体帧头   要写入的数据数组 返回发送邮箱的编号

void can_senddm(CAN_HandleTypeDef* hcan,uint32_t stdid,uint8_t D0,uint8_t D1,uint8_t D2,uint8_t D3,uint8_t D4,uint8_t D5,uint8_t D6,uint8_t D7)
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
//    if (hcan->Instance == CAN2)
//    {
//        HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        a=0;
//        a=1;
//    }

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
        ; // 等待邮箱空闲

    if (hcan->Instance == CAN1)
    {
//        if(stdid==0x200)
//        {
//            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        }
//
//        if(stdid==0x1ff)
//        {
//            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
//        }
        if(stdid==0x102)
        {
            HAL_CAN_AddTxMessage(hcan, &CanTx, Data, &Mailbox);
        }


//        a=0;
//        a=1;
    }


}	//哪个can总线   结构体帧头   要写入的数据数组 返回发送邮箱的编号