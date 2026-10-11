#include "UART4_485_RxTx.h"
#include "bsp_USARTx.h"

HAL_StatusTypeDef UART4_485_SendData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
{
  return HAL_UART_Transmit(huart, data, size, timeout);
}

HAL_StatusTypeDef UART4_485_SendDataDMA(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
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

HAL_StatusTypeDef UART4_485_ReceiveData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size, uint32_t timeout)
{
  return HAL_UART_Receive(huart, data, size, timeout);
}
