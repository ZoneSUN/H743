/**
  ******************************************************************************
  * 文件名程: bsp_debug_usart.h 
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
#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* 宏定义 --------------------------------------------------------------------*/
#define DEBUG_USARTx                              USART1
#define DEBUG_USARTx_BAUDRATE                     115200
#define DEBUG_USART_RCC_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_USARTx_GPIO_AFx                     GPIO_AF7_USART1

#define DEBUG_USARTx_Tx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN                  GPIO_PIN_9
#define DEBUG_USARTx_Tx_GPIO                      GPIOA

#define DEBUG_USARTx_Rx_GPIO_ClK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USARTx_Rx_GPIO_PIN                  GPIO_PIN_10
#define DEBUG_USARTx_Rx_GPIO                      GPIOA

#define DEBUG_USARTx_IRQn                         USART1_IRQn
#define DEBUG_USARTx_IRQHandler                   USART1_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husart_debug;

/* 函数声明 ------------------------------------------------------------------*/
void DEBUG_USARTx_Init(void);

#endif /* __BSP_DEBUG_USART_H__ */

/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
