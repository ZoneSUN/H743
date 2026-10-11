/*
 * @Author: yangwuyang
 * @Date: 2026-09-29 10:00:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-09-29 10:00:00
 * @Description: UART7_485 BSP
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __UART7_485_H__
#define __UART7_485_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "bsp_USARTx.h"
#include <stdio.h>
#include <stdbool.h>

/* PF7 UART7_TX / PF6 UART7_RX */
#define UART7_485_RX_BUFFER_SIZE            18
/* DMA writes this buffer. Storage is padded to a D-Cache line. */
#define UART7_485_TX_BUFFER_SIZE            1024

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef uart7_485_handle;
extern DMA_HandleTypeDef dma_uart7_rx_handle;
extern DMA_HandleTypeDef dma_uart7_tx_handle;
extern uint8_t uart7_rx_buf[UART7_485_RX_BUFFER_SIZE];
extern uint8_t uart7_tx_buf[UART7_485_TX_BUFFER_SIZE];

/* 函数声明 ------------------------------------------------------------------*/
void UART7_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable);

#endif /* __UART7_485_H__ */