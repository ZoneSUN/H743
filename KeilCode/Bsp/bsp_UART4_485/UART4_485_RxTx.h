#ifndef __UART4_485_RXTX_H__
#define __UART4_485_RXTX_H__

#include "stm32h7xx_hal.h"
#include "UART4_485.h"

HAL_StatusTypeDef UART4_485_SendData(UART_HandleTypeDef *huart,
                                     uint8_t *data,
                                     uint16_t size,
                                     uint32_t timeout);
HAL_StatusTypeDef UART4_485_SendDataDMA(UART_HandleTypeDef *huart,
                                        uint8_t *data,
                                        uint16_t size,
                                        uint32_t timeout);
HAL_StatusTypeDef UART4_485_ReceiveData(UART_HandleTypeDef *huart,
                                        uint8_t *data,
                                        uint16_t size,
                                        uint32_t timeout);

#endif /* __UART4_485_RXTX_H__ */
