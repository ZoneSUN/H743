/*
 * @Author: your name
 * @Date: 2026-09-29 14:36:04
 * @LastEditTime: 2026-09-29 14:36:10
 * @LastEditors: your name
 * @Description: 
 * @FilePath: \stm32h7xx_HAL_Driver\Bsp\bsp_USART2_485\USART2_485_RxTx.c
 */
#include "USART2_485_RxTx.h"
#include "bsp_USARTx.h"

#define USART2_485_TX_TIMEOUT  1000U


HAL_StatusTypeDef USART2_485_SendData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
{
  return HAL_UART_Transmit(huart, data, size, timeout);
}

HAL_StatusTypeDef USART2_485_SendDataDMA(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
{
  HAL_StatusTypeDef status;

  (void)timeout;
  status = HAL_UART_Transmit_DMA(huart, data, size);
  if (huart != NULL)
  {
    USARTx_DisableDmaHalfTransfer(huart->hdmatx);
  }
  return status;
}

HAL_StatusTypeDef USART2_485_ReceiveData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
{
  return HAL_UART_Receive(huart, data, size, timeout);
}