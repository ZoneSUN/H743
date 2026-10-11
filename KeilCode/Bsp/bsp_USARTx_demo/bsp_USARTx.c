/*
 * @Author: xujiaofei
 * @Date: 2026-09-17 14:38:00
 * @LastEditors: xujiaofei
 * @LastEditTime: 2026-09-21 10:00:00
 * @Description: 
 * 
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved. 
 */
#include "bsp_USARTx.h"

//配置USARTx内核时钟源
HAL_StatusTypeDef USARTx_PeriphCLK_Config(USART_TypeDef *USARTx, uint32_t clk_source)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit = {0};

  if ((USARTx == USART1) || (USARTx == USART6))
  {
    //PeriphClockSelection: 选择时钟源
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;//USART1/USART6时钟源选择
    //Usart16ClockSelection: 选择USART1/USART6时钟源
    RCC_PeriphClkInit.Usart16ClockSelection = clk_source;
  }
  else if ((USARTx == USART2) || (USARTx == USART3) ||
           (USARTx == UART4) || (USARTx == UART5) ||
           (USARTx == UART7) || (USARTx == UART8))
  {
    //PeriphClockSelection: 选择时钟源
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART234578;//USART2/USART3/UART4/UART5/UART7/UART8时钟源选择
    //Usart234578ClockSelection: 选择USART2/USART3/UART4/UART5/UART7/UART8时钟源
    RCC_PeriphClkInit.Usart234578ClockSelection = clk_source;
  }
  return HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
}

//初始化GPIOx时钟函数
void GPIOx_CLK_Init(GPIO_TypeDef *GPIOx)
{
  if(GPIOx == GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
  }
  else if(GPIOx == GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 使能GPIOB时钟
  }
  else if(GPIOx == GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE(); // 使能GPIOC时钟
  }
  else if(GPIOx == GPIOD)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE(); // 使能GPIOD时钟
  }
  else if(GPIOx == GPIOE)
  {
    __HAL_RCC_GPIOE_CLK_ENABLE(); // 使能GPIOE时钟
  }
  else if(GPIOx == GPIOF)
  {
    __HAL_RCC_GPIOF_CLK_ENABLE(); // 使能GPIOF时钟
  }
  else if(GPIOx == GPIOG)
  {
    __HAL_RCC_GPIOG_CLK_ENABLE(); // 使能GPIOG时钟  
  }
  else if(GPIOx == GPIOH)
  {
    __HAL_RCC_GPIOH_CLK_ENABLE(); // 使能GPIOH时钟
  }
  else if(GPIOx == GPIOI)
  {
    __HAL_RCC_GPIOI_CLK_ENABLE(); // 使能GPIOI时钟
  }
  else if(GPIOx == GPIOJ)
  {
    __HAL_RCC_GPIOJ_CLK_ENABLE(); // 使能GPIOJ时钟
  }
  else if(GPIOx == GPIOK)
  {
    __HAL_RCC_GPIOK_CLK_ENABLE(); // 使能GPIOK时钟
  }
  else
  {
    return;
  }
}

//初始化USARTx时钟函数
void USARTx_USART_CLK_Init(USART_TypeDef *USARTx)
{
  if(USARTx == USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE(); // 使能USART1时钟
  }
  else if(USARTx == USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE(); // 使能USART2时钟
  }
  else if(USARTx == USART3)
  {
    __HAL_RCC_USART3_CLK_ENABLE(); // 使能USART3时钟
  }
  else if(USARTx == UART4)
  {
    __HAL_RCC_UART4_CLK_ENABLE(); // 使能UART4时钟
  }
  else if(USARTx == UART5)
  {
    __HAL_RCC_UART5_CLK_ENABLE(); // 使能UART5时钟
  }
  else if(USARTx == USART6)
  {
    __HAL_RCC_USART6_CLK_ENABLE(); // 使能USART6时钟
  }
  else if(USARTx == UART7)
  {
    __HAL_RCC_UART7_CLK_ENABLE(); // 使能UART7时钟
  }
  else if(USARTx == UART8)
  {
    __HAL_RCC_UART8_CLK_ENABLE(); // 使能UART8时钟
  }
  else{
    return;
  }
  return;
}

//初始化DMA时钟
void DMAx_CLK_Init(DMA_TypeDef *DMAx)
{
  if(DMAx == DMA1)
  {
    __HAL_RCC_DMA1_CLK_ENABLE(); // 使能DMA1时钟
  }
  else if(DMAx == DMA2)
  {
    __HAL_RCC_DMA2_CLK_ENABLE(); // 使能DMA2时钟
  }
  else{
    return;
  }
  return;
}

//初始化USARTx的GPIO函数
HAL_StatusTypeDef USARTx_GPIO_Init(GPIO_TypeDef *GPIOx_tx,                       
                      GPIO_TypeDef *GPIOx_rx, 
                      uint32_t tx_pin, 
                      uint32_t rx_pin,
                      uint32_t tx_af,
                      uint32_t rx_af)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; // 定义GPIO初始化结构体
  
  // 初始化GPIOx时钟
  GPIOx_CLK_Init(GPIOx_tx); // 初始化 TX引脚所在的GPIO时钟
  GPIOx_CLK_Init(GPIOx_rx); // 初始化 RX引脚所在的GPIO时钟

  // 初始化TX引脚
  GPIO_InitStruct.Pin = tx_pin ; // 设置TX引脚
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 设置为复用推挽输出
  GPIO_InitStruct.Pull = GPIO_NOPULL; // 设置为上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 115200 用高速，边沿更陡
  GPIO_InitStruct.Alternate = tx_af; // 设置为复用功能
  HAL_GPIO_Init(GPIOx_tx, &GPIO_InitStruct); // 初始化TX引脚

  // 初始化RX引脚
  GPIO_InitStruct.Pin = rx_pin; // 设置RX引脚
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 设置为复用推挽输出
  GPIO_InitStruct.Pull = GPIO_NOPULL; // 设置为上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 115200 用高速，边沿更陡
  GPIO_InitStruct.Alternate = rx_af; // 设置为复用功能  
  
  HAL_GPIO_Init(GPIOx_rx, &GPIO_InitStruct); // 初始化RX引脚 返回状态
  
  return HAL_OK;
}

//初始化USARTx函数
HAL_StatusTypeDef USARTx_Init(UART_HandleTypeDef *huart, USART_TypeDef *USARTx, uint32_t baudrate)
{
  if ((huart == NULL) || (USARTx == NULL))
  {
    return HAL_ERROR; // 返回错误
  }

  huart->Instance = USARTx; // 设置USARTx寄存器
  huart->Init.BaudRate = baudrate; // 设置波特率
  huart->Init.WordLength = UART_WORDLENGTH_8B; // 设置数据位
  huart->Init.StopBits = UART_STOPBITS_1; // 设置停止位
  huart->Init.Parity = UART_PARITY_NONE; // 设置奇偶校验
  huart->Init.Mode = UART_MODE_TX_RX; // 设置模式
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE; // 硬件流控制
  huart->Init.OverSampling = UART_OVERSAMPLING_16; // 设置过采样
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE; // 设置单比特采样
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; // 高级应用功能
  HAL_StatusTypeDef status = HAL_UART_Init(huart);
  return status;
}

//初始化USARTx的NVIC函数
//中断号：USART2_IRQn
//抢占优先级、子优先级：0~15，0 最高
void USARTx_NVIC_Init(IRQn_Type irq, uint32_t preemptionPriority, uint32_t subPriority)
{
  HAL_NVIC_SetPriority(irq, preemptionPriority, subPriority); // 设置中断优先级
  HAL_NVIC_EnableIRQ(irq); // 使能中断
}

//初始化USARTx的DMA中断函数
void DMAx_NVIC_Init(IRQn_Type irq, uint32_t preemptionPriority, uint32_t subPriority)
{
  HAL_NVIC_SetPriority(irq, preemptionPriority, subPriority); // 设置中断优先级
  HAL_NVIC_EnableIRQ(irq); // 使能中断
}

//初始化USARTx的DMA函数
//H743 使用 DMA Stream 和 DMAMUX 请求，外设地址由 HAL_USART_Receive_DMA / Transmit_DMA 写入 RDR、TDR
HAL_StatusTypeDef USARTx_DMA_Init(UART_HandleTypeDef *huart,
                                  DMA_HandleTypeDef *hdma,
                                  DMA_Stream_TypeDef *stream,
                                  uint32_t request,
                                  uint32_t direction)
{ 
  HAL_StatusTypeDef status;//返回状态
  if ((huart == NULL) || (hdma == NULL) || (stream == NULL)) // 检查句柄是否为空
  {
    return HAL_ERROR; // 返回错误
  }
  if ((direction != DMA_PERIPH_TO_MEMORY) && (direction != DMA_MEMORY_TO_PERIPH)) // 检查方向是否正确
  {
    return HAL_ERROR; // 返回错误
  } 
  hdma->Instance = stream; // 设置DMA流 需长期有效
  hdma->Init.Request = request; // 设置DMAMUX请求
  hdma->Init.Direction = direction; // 设置方向
  hdma->Init.PeriphInc = DMA_PINC_DISABLE; // 设置外设增量
  hdma->Init.MemInc = DMA_MINC_ENABLE; // 设置内存增量
  hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // 设置外设数据对齐
  hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // 设置内存数据对齐
  hdma->Init.Mode = DMA_NORMAL; // 设置模式
  hdma->Init.Priority = DMA_PRIORITY_HIGH; // 设置优先级
  hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 设置FIFO模式
  hdma->Init.MemBurst = DMA_MBURST_SINGLE; // 设置内存突发模式
  hdma->Init.PeriphBurst = DMA_PBURST_SINGLE; //  
  hdma->Parent = huart;  // 设置父句柄
  if (direction == DMA_PERIPH_TO_MEMORY)  // 检查方向是否正确 挂到 hdmarx 
  {
    huart->hdmarx = hdma; // 设置接收DMA句柄 
  }
  else // 检查方向是否正确
  {
    huart->hdmatx = hdma; // 设置发送DMA句柄
  }
  HAL_DMA_DeInit(hdma); // 重置DMA句柄
  status = HAL_DMA_Init(hdma); // 初始化DMA 返回状态
  if (direction == DMA_PERIPH_TO_MEMORY)
  {
    __HAL_LINKDMA(huart, hdmarx, *hdma);
  }
  else
  {
    __HAL_LINKDMA(huart, hdmatx, *hdma);
  }
  
  if (status != HAL_OK)
  {
    return status; // 返回错误  
  }
  return HAL_OK; // 返回成功
}

void USARTx_DisableDmaHalfTransfer(DMA_HandleTypeDef *hdma)
{
  if (hdma == NULL)
  {
    return;
  }
  __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
  __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));
}

HAL_StatusTypeDef USARTx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t size)
{
  HAL_StatusTypeDef status;

  if (huart == NULL)
  {
    return HAL_ERROR;
  }
  status = HAL_UARTEx_ReceiveToIdle_DMA(huart, buf, size);
  USARTx_DisableDmaHalfTransfer(huart->hdmarx);
  return status;
}
