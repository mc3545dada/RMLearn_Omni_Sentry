//
// Created by 3545 on 2024/11/13.
//
#include "chassis.h"
#include "bsp_can.h"
#include "pid.h"
#include <stdio.h>
#include <string.h>
#include "DBUScontrol.h"
#include "tim.h"
#include "motor.h"
#include "communication.h"

#include "bsp_uart.h"
#include "usart.h"
#include "motor_dm.h"

char buffer[128];
int rcflag = 0;
extern int16_t tran_msg1;
extern int16_t tran_msg2;
extern int16_t tran_msg3;
extern int16_t tran_msg4;
extern rc_info_t rc;
extern uint8_t dbus_buf;
int v1=0;
int v2=0;

int angle6020=0;



float Target_dm_angle = 400;

void chassis_init()
{
    Motor_DM_Init(&hcan1, 0x01, 0x02, Motor_DM_Control_Method_NORMAL_ANGLE_OMEGA, 12.5, 30, 10, 10.261194);
    Motor_DM_Send_Exit();
    Target_dm_angle = motor_dm_angle_omega.Control_Angle;

    motor_dm.Motor_DM_Status = Motor_DM_Status_ENABLE;

}


void dm_get_target_angle(float angle)
{
    angle = angle * 0.0005;
    Target_dm_angle -= angle;
    if(Target_dm_angle >= MOTOR_DM_ANGLE_MAX)
    {
        Target_dm_angle = MOTOR_DM_ANGLE_MAX;
    }else if(Target_dm_angle <= -MOTOR_DM_ANGLE_MAX)
    {
        Target_dm_angle = -MOTOR_DM_ANGLE_MAX;
    }
    Motor_DM_Get_Control_Angle(Target_dm_angle);
}


void chassis_ctrl()//在freertos中调用，freertos中的所有进程都只引出函数使用，增加代码可读性
{

//    if(rc.ch0!=0||rc.ch1!=0||rc.ch2!=0||rc.ch3!=0)
//    {
//        rcflag=1;
//    }



    if(motor_dm.Motor_DM_Status == Motor_DM_Status_DISABLE)
    {
        Motor_DM_Send_Exit();
    }else if(motor_dm.Motor_DM_Status == Motor_DM_Status_ENABLE)
    {
        Motor_DM_Send_Enter();
        dm_get_target_angle((-tran_msg2)/12);
        cmd_motor_dm();
    }




//    angle6020=moto_chassis[4].angle/23;   //获取由yaw轴电机得到的角度值
    sprintf(buffer,"%d %d %f %f\r\n",tran_msg1,   //将PID的计算结果通过motor.c函数发送到can总线上，电机接受
            moto_chassis[3].speed_rpm,
            motor_pid[3].target,
            motor_pid[3].output);
    HAL_UART_Transmit_DMA(&huart1,buffer, sizeof(buffer));


//    if(rcflag==1)
//    {





    if(tran_msg3==1)
    {
        v1 = 3200;
    }
    else if(tran_msg3==2)
    {
        v1 = -3200;
    }
    else if(tran_msg3 ==3||tran_msg3==0)
    {
        v1 =0;
    }


        //以电源方向为正方向，电机顺时针转动为正方向，
        motor_pid[0].target =v1;
        motor_pid[0].f_cal_pid(&motor_pid[0],moto_chassis[0].speed_rpm);
//        motor_pid[1].target =0;
//        motor_pid[1].f_cal_pid(&motor_pid[1],moto_chassis[1].speed_rpm);
//        motor_pid[2].target =0;
//        motor_pid[2].f_cal_pid(&motor_pid[2],moto_chassis[2].speed_rpm);
        motor_pid[3].target =-v1;
        motor_pid[3].f_cal_pid(&motor_pid[3],moto_chassis[3].speed_rpm);
//         motor_pid[4].target +=vm;//6020位置环目标（目标角度值）
//         motor_pid[4].f_cal_pid(&motor_pid[4],moto_chassis[4].total_angle);//计算6020位置环pid，取位置环参数，取6020当前总角度为当前pid输入值
//         motor_pid[5].target =motor_pid[4].output;//6020速度环目标（位置环输出的目标速度）
//         motor_pid[5].f_cal_pid(&motor_pid[5],moto_chassis[4].speed_rpm);//计算6020速度环pid，取速度环参数，取6020当前速度为当前pid输入值
        motor_pid[6].target =-v1/3;
        motor_pid[6].f_cal_pid(&motor_pid[6],moto_chassis[6].speed_rpm);
//        motor_pid[7].target =0;
//        motor_pid[7].f_cal_pid(&motor_pid[1],moto_chassis[1].speed_rpm);





            set_chassis_current(&hcan1,  0x200, motor_pid[0].output,   //将PID的计算结果通过motor.c函数发送到can总线上，电机接受     //发射击电机数据
                                0,
                                0,
                                motor_pid[3].output);


            set_chassis_current(&hcan1,  0x1ff, 0,   //将PID的计算结果通过motor.c函数发送到can总线上，电机接受     //发2006数据
                                0,
                                motor_pid[6].output,
                                0);



//             set_chassis_current(&hcan2,  0x1ff, motor_pid[5].output, //输入6020速度环得到的速度值       //发yaw轴电机数据
//                                              0,
//                                              0,
//                                              0);

//    }



//            set_tran_msg(&hcan2,0x345,angle6020,rc.ch1,rc.roll,0);                //发双板通信数据






}