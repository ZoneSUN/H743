/**
  ******************************************************************************
  * 文件名程: bsp_RS485.h 
  * 作    者: MZXQ嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 
  * 功    能: UART-API
  ******************************************************************************
  * 说明：
  * 本例程配套MZXQstm32开发板使用。
  * 
  * 淘宝：
  * 论坛：MZXQ
  * 版权归MZXQ嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
#ifndef __BSP_RS485_H__
#define __BSP_RS485_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* 宏定义 --------------------------------------------------------------------*/
#define MX_RS485_1_UARTx                              UART7
#define MX_RS485_1_UARTx_BAUDRATE                     115200
#define MX_RS485_1_UART_RCC_CLK_ENABLE()              __HAL_RCC_UART7_CLK_ENABLE()
#define MX_RS485_1_UARTx_GPIO_AFx                     GPIO_AF7_UART7

#define MX_RS485_1_UARTx_Tx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define MX_RS485_1_UARTx_Tx_GPIO_PIN                  GPIO_PIN_7
#define MX_RS485_1_UARTx_Tx_GPIO                      GPIOF

#define MX_RS485_1_UARTx_Rx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define MX_RS485_1_UARTx_Rx_GPIO_PIN                  GPIO_PIN_6
#define MX_RS485_1_UARTx_Rx_GPIO                      GPIOF

#define MX_RS485_1_UARTx_IRQn                         UART7_IRQn
#define MX_RS485_1_UARTx_IRQHandler                   UART7_IRQHandler


#define MX_RS485_2_UARTx                              USART2
#define MX_RS485_2_UARTx_BAUDRATE                     115200
#define MX_RS485_2_UART_RCC_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define MX_RS485_2_UARTx_GPIO_AFx                     GPIO_AF7_USART2

#define MX_RS485_2_UARTx_Tx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define MX_RS485_2_UARTx_Tx_GPIO_PIN                  GPIO_PIN_5
#define MX_RS485_2_UARTx_Tx_GPIO                      GPIOD

#define MX_RS485_2_UARTx_Rx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define MX_RS485_2_UARTx_Rx_GPIO_PIN                  GPIO_PIN_6
#define MX_RS485_2_UARTx_Rx_GPIO                      GPIOD

#define MX_RS485_2_UARTx_IRQn                         USART2_IRQn
#define MX_RS485_2_UARTx_IRQHandler                   USART2_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef huartx_RS485_1;
extern UART_HandleTypeDef huartx_RS485_2;

/* 函数声明 ------------------------------------------------------------------*/
void MX_RS485_1_UARTx_Init(void);
void MX_RS485_2_UARTx_Init(void);

#endif /* __BSP_RS485_H__ */

/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
