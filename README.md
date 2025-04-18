# RoboMaster

一个为 RoboMaster 项目创建的仓库。

这是由 `mc3545dada` 制作的 RM-omni-sentinel 版本，具备控制底盘和云台的基本功能。

该机器人使用两个 C 板，分别用于底盘和云台控制。

---

## 🛠️ 开发环境

本项目使用以下工具进行开发：

- [CLion](https://www.jetbrains.com/clion/)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

---

## 🚗 底盘（Chassis）

在底盘部分，MCU 与以下设备连接：

- **4 个 M3508 电机** + **1 个 C620 电调** （CAN1，总线 ID：1, 2, 3, 4）用于移动控制  
- **1 个 GM6020 电机**（CAN2，ID：1）用于 Yaw 轴控制  
- **1 个 DR16 接收机**：通过 DT 协议接收遥控器数据

---

## 🎯 云台（Gimbal）

在云台部分，MCU 与以下设备连接：

- **1 个 4310 电机**（CAN1）：控制 Pitch 轴  
- **1 个 M2006 电机** + **1 个 C610 电调**（CAN1）：控制 17mm 弹丸送料器  
- **2 个 M3508 电机** + **1 个 C620 电调**（CAN1）：控制 17mm 弹丸射击装置  

---

## 📌 说明（Notes）

- 本版本主要用于训练目的，基于 STM32F407IGHX 的基本功能实现。
- 项目中可能存在 **Bug** 或 **代码维护不佳/结构不合理** 的情况。
- 本项目旨在纪念 `mc3545dada` 进入 RoboMaster 领域的起点。

---
---
---

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
