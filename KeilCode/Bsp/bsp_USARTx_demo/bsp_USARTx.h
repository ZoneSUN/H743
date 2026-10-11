/*
 * @Author: yangwuyang
 * @Date: 2026-09-17 14:38:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-09-17 14:38:13
 * @Description: USARTx BSP demo
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __BSP_USARTX_H__
#define __BSP_USARTX_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"                  // Device header


//配置USARTx内核时钟源
/**
 * @brief 配置 USARTx 内核时钟源
 * @param USARTx: USARTx 寄存器
 * @param clk_source: 时钟源
 *        USART1/USART6: RCC_USART16CLKSOURCE_D2PCLK2、PLL2、PLL3、HSI、CSI、LSE
 *        USART2/3、UART4/5/7/8: RCC_USART234578CLKSOURCE_D2PCLK1、PLL2、PLL3、HSI、CSI、LSE
 * @return HAL_StatusTypeDef
 * @example
 * USARTx_PeriphCLK_Config(USART1, RCC_USART16CLKSOURCE_D2PCLK2);
 */
  inline HAL_StatusTypeDef USARTx_PeriphCLK_Config(USART_TypeDef *USARTx, uint32_t clk_source);

//初始化GPIOx时钟函数
/**
 * @brief 初始化 GPIOx 时钟
 * @param GPIOx: GPIO端口
 * @return: 无
 * @note: 初始化 GPIOx 时钟，需要先使能 GPIOx 时钟
 * @example:
 * GPIOx_CLK_Init(GPIOA);
 */
  void GPIOx_CLK_Init(GPIO_TypeDef *GPIOx);

//初始化USARTx时钟函数
/**
 * @brief 初始化 USARTx 时钟
 * @param USARTx: USARTx 寄存器
 * @return: 无
 * @note: 初始化 USARTx 时钟，需要先使能 USARTx 时钟
 * @example:
 * USARTx_USART_CLK_Init(USART3);
 */
  void USARTx_USART_CLK_Init(USART_TypeDef *USARTx);

//初始化DMA时钟
/**
 * @brief 初始化 DMA 时钟
 * @param DMAx: DMAx 寄存器
 * @return: 无
 * @note: 初始化 DMA 时钟，需要先使能 DMA 时钟
 * @example:
 * DMAx_CLK_Init(DMA1);
 */
  void DMAx_CLK_Init(DMA_TypeDef *DMAx);



/**
 * @brief 初始化 USARTx 的 GPIO
 * @param GPIOx_tx: TX引脚所在的GPIO端口
 * @param GPIOx_rx: RX引脚所在的GPIO端口
 * @param tx_pin: TX引脚
 * @param rx_pin: RX引脚
 * @param tx_af: TX引脚的复用功能
 * @param rx_af: RX引脚的复用功能
 * @return: HAL_StatusTypeDef 返回状态  HAL_OK 成功，HAL_ERROR 失败  HAL_BUSY 忙  HAL_TIMEOUT 超时  HAL_UNSUPPORTED 不支持
 * @note: 初始化 USARTx 的 GPIO，需要先使能 GPIO 时钟
 * @example:
 * USARTx_GPIO_Init(GPIOA, GPIOB, GPIO_PIN_9, GPIO_PIN_10, GPIO_AF7_USART3, GPIO_AF7_USART3);
 */
  inline HAL_StatusTypeDef USARTx_GPIO_Init(
                      GPIO_TypeDef *GPIOx_tx,
                      GPIO_TypeDef *GPIOx_rx,
                      uint32_t tx_pin,
                      uint32_t rx_pin,
                      uint32_t tx_af,
                      uint32_t rx_af);

/**
 * @brief 初始化 USARTx
 * @param huart: UART 句柄，内部的 Instance 和 Init 由本函数填写
 * @param USARTx: USARTx 寄存器，例如 USART2
 * @param baudrate: 波特率
 * @return HAL_StatusTypeDef
 * @note 调用前需先使能 USARTx 时钟。huart 需保持有效，后续收发都用这个句柄
 * @example
 * UART_HandleTypeDef huart2;
 * USARTx_Init(&huart2, USART2, 115200);
 */
  inline HAL_StatusTypeDef USARTx_Init(UART_HandleTypeDef *huart, USART_TypeDef *USARTx, uint32_t baudrate);

//串口中断设置
/**
 * @brief 初始化 USARTx 的 NVIC
 * @param irq: 中断号，例如 USART2_IRQn
 * @param preemptionPriority: 抢占优先级，0~15，0 最高
 * @param subPriority: 子优先级，0~15，0 最高
 * @return 无
 * @note 使用 HAL_NVIC_SetPriority 和 HAL_NVIC_EnableIRQ
 * @example
 * USARTx_NVIC_Init(USART2_IRQn, 0, 0);
 */
  inline void USARTx_NVIC_Init(IRQn_Type irq,
                      uint32_t preemptionPriority,
                      uint32_t subPriority);  
//DMA中断设置
/**
 * @brief 初始化 USARTx 的 DMA 中断
 * @param irq: 中断号，例如 DMA1_Stream0_IRQn
 * @param preemptionPriority: 抢占优先级，0~15，0 最高
 * @param subPriority: 子优先级，0~15，0 最高
 * @return: 无   使用 HAL_NVIC_SetPriority 和 HAL_NVIC_EnableIRQ  
 * @example:
 * DMAx_NVIC_Init(DMA1_Stream0_IRQn, 0, 0);
 */
  inline void DMAx_NVIC_Init(IRQn_Type irq,    
                    uint32_t preemptionPriority,
                    uint32_t subPriority);
/**
 * @brief 初始化 USARTx 的 DMA，并挂到 USART 句柄
 * @param husart: 已初始化的 USART 句柄 需长期有效 
 * @param hdma: DMA 句柄，需长期有效
 * @param stream: DMA 流，例如 DMA1_Stream0
 * @param request: DMAMUX 请求，例如 DMA_REQUEST_USART2_RX
 * @param direction: DMA_PERIPH_TO_MEMORY 挂到 hdmarx，DMA_MEMORY_TO_PERIPH 挂到 hdmatx
 * @return HAL_StatusTypeDef
 * @example
 * USARTx_DMA_Init(&husart2, &hdma_rx, DMA1_Stream0, DMA_REQUEST_USART2_RX, DMA_PERIPH_TO_MEMORY);
 */
  inline HAL_StatusTypeDef USARTx_DMA_Init(UART_HandleTypeDef *huart,
                                  DMA_HandleTypeDef *hdma,
                                  DMA_Stream_TypeDef *stream,
                                  uint32_t request,
                                  uint32_t direction);

/**
 * @brief 启动 DMA 收至空闲，并关闭半满中断
 * @note HAL 启动时会打开 HT。空闲与传输完成中断保留。
 */
HAL_StatusTypeDef USARTx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t size);

/* 关闭指定 DMA 的半满中断并清掉已挂起的半满标志 */
void USARTx_DisableDmaHalfTransfer(DMA_HandleTypeDef *hdma);


#endif /* __BSP_USARTX_H__ */
