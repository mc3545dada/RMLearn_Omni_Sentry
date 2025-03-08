# RoboMaster
a repository of RoboMaster
this is a RM-omni-sentinel version made by mc3545dada
it have rudimentary function to control the chassis and gimbal
the robot use 2*C-board in chassis and gimbal

use Clion+Stm32Cubemx to code

at chassis,MCU connect 4*M3508&C620(can1,id:1 2 3 4) to control  move and 1*GM6020(can2,id:1) to control its yaw motor, and 1*DR16 to receive the remote-control message by DT

at gimbal,MCU connect 1*4310Motor(can1) to control pitch motor ,1*M2006&C610(can1) to Control 17mm provide and 2*M3508&C620(can1) to control 17mm shoot


at last,that just a version for training use the elementary function of STM32F407IGHX,so it maybe has lots of bugs and some place which is lack of elegant and well-maintained code

This is dedicated to commemorate the beginning of mc3545dada's journey into RoboMaster
