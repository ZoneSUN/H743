/*
 * @Author: your name
 * @Date: 2026-09-29 14:36:04
 * @LastEditTime: 2026-09-29 14:36:10
 * @LastEditors: your name
 * @Description: 
 * @FilePath: \stm32h7xx_HAL_Driver\Bsp\bsp_USART2_485\USART2_485_RxTx.h
 */
#ifndef __USART2_485_RXTX_H__
#define __USART2_485_RXTX_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "USART2_485.h"

/**
 * @name USART2_485_SendData
 * @brief USART2_485 阻塞发送
 * @param husart USART 句柄
 * @param data 发送数据
 * @param size 发送字节数
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef USART2_485_SendData(UART_HandleTypeDef *huart,
                                      uint8_t *data,
                                      uint16_t size,
                                      uint32_t timeout);

/**
 * @name USART2_485_SendDataDMA
 * @brief USART2_485 DMA 发送
 * @param husart USART 句柄
 * @param data 发送数据
 * @param size 发送字节数
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef USART2_485_SendDataDMA(UART_HandleTypeDef *huart,
                                         uint8_t *data,
                                         uint16_t size,
                                         uint32_t timeout);

/**
 * @name USART2_485_ReceiveData
 * @brief USART2_485 阻塞接收
 * @param huart USART 句柄
 * @param data 接收缓冲区
 * @param size 期望字节数
 * @param timeout 超时，单位 ms
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef USART2_485_ReceiveData(UART_HandleTypeDef *huart,
                                         uint8_t *data,
                                         uint16_t size,
                                         uint32_t timeout);

#endif /* __USART2_485_RXTX_H__ */
