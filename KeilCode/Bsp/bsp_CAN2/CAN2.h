/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:23:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:23:00
 * @Description: CAN2 BSP，对应开发板 FDCAN2
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __CAN2_H__
#define __CAN2_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "bsp_CANFDx.h"
#include <stdio.h>

/* PB6 FDCAN2_TX / PB5 FDCAN2_RX */
#define CAN2_RX_BUFFER_SIZE                 8U

extern FDCAN_HandleTypeDef can2_handle;
extern FDCAN_RxHeaderTypeDef can2_rx_header;
extern uint8_t can2_rx_buf[CAN2_RX_BUFFER_SIZE];

/* baudrate: 名义位速率，例如 500000 表示 500kbps。需先配置 FDCAN 内核时钟 */
void CAN2_Start(uint32_t baudrate);

/* id 为 11 位标准帧 ID。四个参数各取低 8 位，组成 4 字节经典 CAN 数据帧。 */
HAL_StatusTypeDef CAN2_Send(uint32_t id, uint32_t data0, uint32_t data1, uint32_t data2);

#endif /* __CAN2_H__ */
