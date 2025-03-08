RoboMaster
A repository for RoboMaster.

This is the RM-omni-sentinel version created by mc3545dada.

It has rudimentary functionality to control the chassis and gimbal.

The robot uses 2 C-boards for the chassis and gimbal.

Development Environment
The code is developed using CLion and STM32CubeMX.

Chassis
In the chassis, the MCU is connected to:

4 * M3508 motors and 1 * C620 motor (CAN1, IDs: 1, 2, 3, 4) to control movement.
1 * GM6020 motor (CAN2, ID: 1) to control the yaw motor.
1 * DR16 receiver to receive remote control messages via DT.
Gimbal
In the gimbal, the MCU is connected to:

1 * 4310 motor (CAN1) to control the pitch motor.
1 * M2006 motor and 1 * C610 motor (CAN1) to control the 17mm feeder.
2 * M3508 motors and 1 * C620 motor (CAN1) to control the 17mm shooter.
Notes
This version is primarily for training purposes, utilizing the basic functionality of the STM32F407IGHX.

It may contain bugs and areas with suboptimal or poorly maintained code.

This project is dedicated to commemorate the beginning of mc3545dada's journey into RoboMaster.
