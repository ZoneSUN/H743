/*
 * @Description: µ×°åÀ©Õ¹ 485-3£¬USART1
 * PB14 USART1_TX / PB15 USART1_RX£¬¸´ÓÃ AF4
 */
#ifndef __USART1_485_H__
#define __USART1_485_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "bsp_USARTx.h"
#include <stdio.h>
#include <stdbool.h>

#define USART1_485_RX_BUFFER_SIZE           18
#define USART1_485_TX_BUFFER_SIZE           1024

extern UART_HandleTypeDef usart1_485_handle;
extern DMA_HandleTypeDef dma_usart1_rx_handle;
extern DMA_HandleTypeDef dma_usart1_tx_handle;
extern uint8_t usart1_rx_buf[USART1_485_RX_BUFFER_SIZE];
extern uint8_t usart1_tx_buf[USART1_485_TX_BUFFER_SIZE];

void USART1_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable);

#endif /* __USART1_485_H__ */
