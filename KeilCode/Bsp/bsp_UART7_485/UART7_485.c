/*
 * @Author: yangwuyang
 * @Date: 2026-09-29 10:00:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-09-29 10:00:00
 * @Description: UART7_485 BSP  
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#include "UART7_485.h"

UART_HandleTypeDef uart7_485_handle;
DMA_HandleTypeDef dma_uart7_rx_handle;
DMA_HandleTypeDef dma_uart7_tx_handle;

uint8_t uart7_rx_buf[UART7_485_RX_BUFFER_SIZE] __attribute__((at(0x24000000), zero_init));
uint8_t uart7_tx_buf[UART7_485_TX_BUFFER_SIZE] __attribute__((at(0x24000020), zero_init));

void UART7_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable)
{
  HAL_StatusTypeDef clk_status = USARTx_PeriphCLK_Config(UART7, RCC_USART234578CLKSOURCE_D2PCLK1);
  printf("UART7_485_Start status: %d, PCLK1=%luM\r\n",
         (int)clk_status, (unsigned long)HAL_RCC_GetPCLK1Freq()/1000000);

  /* PF7 TX / PF6 RX */
  HAL_StatusTypeDef gpio_status = USARTx_GPIO_Init(GPIOF, GPIOF, GPIO_PIN_7, GPIO_PIN_6, GPIO_AF7_UART7, GPIO_AF7_UART7);
  USARTx_USART_CLK_Init(UART7);
  HAL_StatusTypeDef uart_status = USARTx_Init(&uart7_485_handle, UART7, baudrate);
  (void)gpio_status;
  (void)uart_status;

  if ((DMA_tx_Enable != false) || (DMA_rx_Enable != false))
  {
    DMAx_CLK_Init(DMA1);
  }

  if (DMA_rx_Enable != false)
  {
    HAL_StatusTypeDef dma_rx_status = USARTx_DMA_Init(&uart7_485_handle,
                                                      &dma_uart7_rx_handle,
                                                      DMA1_Stream3,
                                                      DMA_REQUEST_UART7_RX,
                                                      DMA_PERIPH_TO_MEMORY);
    printf("UART7 DMA RX status: %d\r\n", (int)dma_rx_status);
  }
  if (DMA_tx_Enable != false)
  {
    HAL_StatusTypeDef dma_tx_status = USARTx_DMA_Init(&uart7_485_handle,
                                                      &dma_uart7_tx_handle,
                                                      DMA1_Stream2,
                                                      DMA_REQUEST_UART7_TX,
                                                      DMA_MEMORY_TO_PERIPH);
    printf("UART7 DMA TX status: %d\r\n", (int)dma_tx_status);
  }
  if (DMA_rx_Enable != false && DMA_tx_Enable != false)//DMA_rx和DMA_tx都使能
  {
    DMAx_NVIC_Init(DMA1_Stream3_IRQn, 2, 0);
    DMAx_NVIC_Init(DMA1_Stream2_IRQn, 2, 0);
    USARTx_NVIC_Init(UART7_IRQn, 2, 0);
    USARTx_ReceiveToIdle_DMA(&uart7_485_handle, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
  }
  else if (DMA_rx_Enable != false && DMA_tx_Enable == false)//DMA_rx使能，DMA_tx不使能
  {
    DMAx_NVIC_Init(DMA1_Stream3_IRQn, 2, 0);
    USARTx_NVIC_Init(UART7_IRQn, 2, 0);
    USARTx_ReceiveToIdle_DMA(&uart7_485_handle, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
  }
  else if (DMA_rx_Enable == false && DMA_tx_Enable != false)//DMA_rx不使能，DMA_tx使能
  {
    DMAx_NVIC_Init(DMA1_Stream2_IRQn, 2, 0);
    USARTx_NVIC_Init(UART7_IRQn, 2, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&uart7_485_handle, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
  }
  else//DMA_rx和DMA_tx都不使能
  {
    USARTx_NVIC_Init(UART7_IRQn, 2, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&uart7_485_handle, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
  }
}

//UART7_IRQHandler 中断处理函数
void UART7_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart7_485_handle);//UART7_485 中断处理函数作用
}
//DMA1_Stream3_IRQHandler 中断处理函数
void DMA1_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart7_rx_handle);//DMA1_Stream3 中断处理函数
}

void DMA1_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart7_tx_handle);//DMA1_Stream2 中断处理函数
}
