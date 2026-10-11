
/** 
 * @file USART2_485.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
 #ifndef __USART2_485_H__
 #define __USART2_485_H__


 //PD6_485_USART2_RX	
 //PD5_485_USART2_TX


 /* 包含头文件 ----------------------------------------------------------------*/
 #include "stm32h7xx_hal.h"
 #include <stdio.h>
 #include <stdbool.h>
 #include "bsp_USARTx.h"
 /* 宏定义 ------------------------------------------------------------------*/
 #define USART2_485_RX_BUFFER_SIZE 18
 #define USART2_485_TX_BUFFER_SIZE 1024

 /* 类型定义 ------------------------------------------------------------------*/
 
 /* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef usart2_485_handle;
extern DMA_HandleTypeDef dma_usart2_rx_handle;
extern DMA_HandleTypeDef dma_usart2_tx_handle;
extern uint8_t usart2_rx_buf[USART2_485_RX_BUFFER_SIZE];
 /* 函数声明 ------------------------------------------------------------------*/
void USART2_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable);
//中断接受开启函数



 
 
 #endif
