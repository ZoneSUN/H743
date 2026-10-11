/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:23:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:23:00
 * @Description: CAN1 BSP，对应开发板 FDCAN1
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __CAN1_H__
#define __CAN1_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "bsp_CANFDx.h"
#include <stdio.h>

/* PB9 FDCAN1_TX / PB8 FDCAN1_RX */
#define CAN1_RX_BUFFER_SIZE                 8U

extern FDCAN_HandleTypeDef can1_handle;
extern FDCAN_RxHeaderTypeDef can1_rx_header;
extern uint8_t can1_rx_buf[CAN1_RX_BUFFER_SIZE];

/* baudrate: 名义位速率，例如 500000 表示 500kbps。需先配置 FDCAN 内核时钟 */
void CAN1_Start(uint32_t baudrate);

#endif /* __CAN1_H__ */
