/*
 * @Description: µ×°åÀ©Õ¹ 485-4£¬UART4
 * PH13 UART4_TX / PH14 UART4_RX£¬¸´ÓÃ AF8
 */
#ifndef __UART4_485_H__
#define __UART4_485_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "bsp_USARTx.h"
#include <stdio.h>
#include <stdbool.h>

#define UART4_485_RX_BUFFER_SIZE            18
#define UART4_485_TX_BUFFER_SIZE            1024

extern UART_HandleTypeDef uart4_485_handle;
extern DMA_HandleTypeDef dma_uart4_rx_handle;
extern DMA_HandleTypeDef dma_uart4_tx_handle;
extern uint8_t uart4_rx_buf[UART4_485_RX_BUFFER_SIZE];
extern uint8_t uart4_tx_buf[UART4_485_TX_BUFFER_SIZE];

void UART4_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable);

#endif /* __UART4_485_H__ */
