//
// Created by 3545 on 2024/11/15.
//
#include "Daemon.h"
#include "main.h"
#include "cmsis_os2.h"
#include "pid.h"
#include "bsp_can.h"
#include "DBUScontrol.h"

extern rc_info_t rc;
int offlinecount=0;


void daemon_cntreset()//离线计数清零函数，在bspcan接受中断回调中调用此函数
{
    offlinecount=0;
}

void daemon_task()
{
    offlinecount++;
    if(offlinecount>=10)
    {
        //////pid清零
        motor_pid[0].f_param_init(&motor_pid[0],PID_Speed,16384,5000,10,0,8000,0,0,0,0);
        motor_pid[1].f_param_init(&motor_pid[1],PID_Speed,16384,5000,10,0,8000,0,0,0,0);
        motor_pid[2].f_param_init(&motor_pid[2],PID_Speed,16384,5000,10,0,8000,0,0,0,0);
        motor_pid[3].f_param_init(&motor_pid[3],PID_Speed,16384,5000,10,0,8000,0,0,0,0);

        if(moto_chassis[0].real_current!=0||moto_chassis[1].real_current!=0||moto_chassis[2].real_current!=0||moto_chassis[3].real_current!=0) {
            offlinecount = 0;

            motor_pid[0].f_param_init(&motor_pid[0], PID_Speed, 16384, 5000, 10, 0, 8000, 0, 1.8, 0.05, 0);
            motor_pid[1].f_param_init(&motor_pid[1], PID_Speed, 16384, 5000, 10, 0, 8000, 0, 1.8, 0.05, 0);
            motor_pid[2].f_param_init(&motor_pid[2], PID_Speed, 16384, 5000, 10, 0, 8000, 0, 1.8, 0.05, 0);
            motor_pid[3].f_param_init(&motor_pid[3], PID_Speed, 16384, 5000, 10, 0, 8000, 0, 1.8, 0.05, 0);
        }

    }



    osDelay(100 - 1);
}



