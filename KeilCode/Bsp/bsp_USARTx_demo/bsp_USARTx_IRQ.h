/*
 * @Author: yangwuyang
 * @Date: 2026-09-28 10:04:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-09-29 09:07:00
 * @Description: USARTx interrupt callback
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __BSP_USARTX_IRQ_H__
#define __BSP_USARTX_IRQ_H__

#include "stm32h7xx_hal.h"

extern __IO uint8_t  USARTx_TxHalfCpltFlag;
extern __IO uint8_t  USARTx_TxCpltFlag;
extern __IO uint8_t  USARTx_RxHalfCpltFlag;
extern __IO uint8_t  USARTx_RxCpltFlag;
extern __IO uint8_t  USARTx_AbortCpltFlag;
extern __IO uint32_t USARTx_ErrorCode;

void USARTx_IRQHandler(UART_HandleTypeDef *huart);
void USARTx_DMA_IRQHandler(DMA_HandleTypeDef *hdma);

#endif /* __BSP_USARTX_IRQ_H__ */
