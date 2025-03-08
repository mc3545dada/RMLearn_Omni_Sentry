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
#include <math.h>
#include "bsp_uart.h"
#include "usart.h"
char buffer[128];
int rcflag = 0;
extern int16_t tran_msg1;
extern int16_t tran_msg2;
extern int16_t tran_msg3;
extern int16_t tran_msg4;
extern rc_info_t rc;
extern uint8_t dbus_buf;
int vx=0;
int vy=0;
int vg=0;
int vc=0;
int angle6020=0;
int rtcnt=0;
int vxdc=0;
int vxdg=0;



double sin_degrees(double degrees)
{
    // 将角度转换为弧度
    double radians = degrees * (M_PI / 180.0);
    // 计算正弦值并返回
    return sin(radians);
}



void chassis_init()
{


}

void chassis_ctrl()//在freertos中调用，freertos中的所有进程都只引出函数使用，增加代码可读性
{
//    if(rc.ch0!=0||rc.ch1!=0||rc.ch2!=0||rc.ch3!=0)
//    {
//        rcflag=1;
//    }


//    rtcnt++;
//    if(rc.ch0!=0)
//    {
//        rtcnt=0;
//    }
//        if(rtcnt>100)
//        {
//            motor_pid[4].target =0;
//        }

    angle6020=(moto_chassis[4].angle/22.7);
    sprintf(buffer, "%d %d %d %d\r\n", angle6020,   //将PID的计算结果通过motor.c函数发送到can总线上，电机接受
            vc,
            rc.mouse.l,
            0);
    HAL_UART_Transmit_DMA(&huart1,buffer, sizeof(buffer));


//    if(rcflag==1)
//    {

//        if(angle6020>=185&&angle6020<=335)
//            {
//                vc=1200;
//
//            }
//        else if(angle6020>=20&&angle6020<=175)
//            {
//                vc=-1200;
//
//            }
//        else
//            {
//                vc=0;
//            }

        vx=3*rc.ch3;
        vy=3*rc.ch2;
        vg= (-rc.ch0) / 60;
        vc= 5 * (-rc.roll);


        if(rc.sw1==1)
        {
            vxdc=1560;
            vxdg=3;

        }
        else if(rc.sw1 ==3)
        {
            vxdc=0;
            vxdg=0;

        }
        else
        {
            vxdc=-2600;
            vxdg=-5;

        }


//    motor_pid[0].target =(-0.7*vx)+(sin(45/*+jiaodu*/)*vy)+vc; 45 135 225 315     - + + -

        //以电源方向为正方向，电机顺时针转动为正方向，
        motor_pid[0].target = (sin_degrees(315-angle6020)*vx) + (sin_degrees(45-angle6020)*vy) + vc+vxdc;               //右上
        motor_pid[0].f_cal_pid(&motor_pid[0],moto_chassis[0].speed_rpm);
        motor_pid[1].target = (sin_degrees(45-angle6020)*vx) + (sin_degrees(135-angle6020)*vy) + vc+vxdc;               //右下
        motor_pid[1].f_cal_pid(&motor_pid[1],moto_chassis[1].speed_rpm);
        motor_pid[2].target = (sin_degrees(135-angle6020)*vx) + (sin_degrees(225-angle6020)*vy) + vc+vxdc;                //左下
        motor_pid[2].f_cal_pid(&motor_pid[2],moto_chassis[2].speed_rpm);
        motor_pid[3].target = (sin_degrees(225-angle6020)*vx) + (sin_degrees(315-angle6020)*vy) + vc+vxdc;                 //左上
        motor_pid[3].f_cal_pid(&motor_pid[3],moto_chassis[3].speed_rpm);

         motor_pid[4].target +=(vg) +=(vxdg);//6020位置环目标（目标角度值）
         motor_pid[4].f_cal_pid(&motor_pid[4],moto_chassis[4].total_angle);//计算6020位置环pid，取位置环参数，取6020当前总角度为当前pid输入值
         motor_pid[5].target =motor_pid[4].output;//6020速度环目标（位置环输出的目标速度）
         motor_pid[5].f_cal_pid(&motor_pid[5],moto_chassis[4].speed_rpm);//计算6020速度环pid，取速度环参数，取6020当前速度为当前pid输入值


            set_chassis_current(&hcan1,  0x200, motor_pid[0].output,   //将PID的计算结果通过motor.c函数发送到can总线上，电机接受
                                motor_pid[1].output,
                                motor_pid[2].output,
                                motor_pid[3].output);


             set_chassis_current(&hcan2,  0x1ff, motor_pid[5].output, //输入6020速度环得到的速度值
                                              0,
                                              0,
                                              0);
//    }



            set_tran_msg(&hcan2,0x345,angle6020,rc.ch1,rc.sw2,0);
}