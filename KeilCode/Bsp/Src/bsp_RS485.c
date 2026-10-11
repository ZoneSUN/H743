/**
  ******************************************************************************
  * 文件名程: bsp_RS485.c 
  * 作    者: MZXQ嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 
  * 功    能: 串口通信功能实现
  ******************************************************************************
  * 说明：
  * 本例程配套MZXQstm32开发板使用。
  * 
  * 淘宝：
  * 论坛：MZXQ
  * 版权归MZXQ嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_RS485.h"

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
UART_HandleTypeDef huartx_RS485_1;
UART_HandleTypeDef huartx_RS485_2;

/* 扩展变量 ------------------------------------------------------------------*/

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载串口引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口引脚
  */
static void YS_RS485_1_UARTx_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};

  /* USART 时钟使能 */
  MX_RS485_1_UART_RCC_CLK_ENABLE();
  /* 引脚 时钟使能 */
  MX_RS485_1_UARTx_Tx_GPIO_ClK_ENABLE();
	MX_RS485_1_UARTx_Rx_GPIO_ClK_ENABLE();
  
  /* USART GPIO 配置: 复用功能模式  */
  GPIO_InitStruct.Pin = MX_RS485_1_UARTx_Tx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = MX_RS485_1_UARTx_GPIO_AFx;
  HAL_GPIO_Init(MX_RS485_1_UARTx_Tx_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = MX_RS485_1_UARTx_Rx_GPIO_PIN;
  HAL_GPIO_Init(MX_RS485_1_UARTx_Rx_GPIO, &GPIO_InitStruct);
    
  /* NVIC中设置串口中断优先级*/
  HAL_NVIC_SetPriority(MX_RS485_1_UARTx_IRQn, 2, 0);
	/* NVIC中使能串口中断*/
  HAL_NVIC_EnableIRQ(MX_RS485_1_UARTx_IRQn);
}

/**
  * 函数功能: 板载串口通信初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口通信功能
  */
void MX_RS485_1_UARTx_Init(void)
{
  /* 串口通信GPIO初始化 */
  YS_RS485_1_UARTx_MspInit();
  /* 串口通信功能配置 */
  huartx_RS485_1.Instance = MX_RS485_1_UARTx;                                   // 串口号
  huartx_RS485_1.Init.BaudRate   = MX_RS485_1_UARTx_BAUDRATE;                   // 波特率
  huartx_RS485_1.Init.WordLength = UART_WORDLENGTH_8B;                   // 有效数据长度
  huartx_RS485_1.Init.StopBits   = UART_STOPBITS_1;                      // 停止位
  huartx_RS485_1.Init.Parity     = UART_PARITY_NONE;                     // 奇偶校验
  huartx_RS485_1.Init.Mode       = UART_MODE_TX_RX;                      // 发送&接收模式
  huartx_RS485_1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;                  // 硬件流控制
  huartx_RS485_1.Init.OverSampling    = UART_OVERSAMPLING_16;            // 过采样
  huartx_RS485_1.Init.OneBitSampling  = UART_ONE_BIT_SAMPLE_DISABLE;     // 采样位
  huartx_RS485_1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;  // 高级应用功能
  HAL_UART_Init(&huartx_RS485_1);
}

/**
  * 函数功能: 板载串口引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口引脚
  */
static void YS_RS485_2_UARTx_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};

  /* USART 时钟使能 */
  MX_RS485_2_UART_RCC_CLK_ENABLE();
  /* 引脚 时钟使能 */
  MX_RS485_2_UARTx_Tx_GPIO_ClK_ENABLE();
	MX_RS485_2_UARTx_Rx_GPIO_ClK_ENABLE();
  
  /* USART GPIO 配置: 复用功能模式  */
  GPIO_InitStruct.Pin = MX_RS485_2_UARTx_Tx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = MX_RS485_2_UARTx_GPIO_AFx;
  HAL_GPIO_Init(MX_RS485_2_UARTx_Tx_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = MX_RS485_2_UARTx_Rx_GPIO_PIN;
  HAL_GPIO_Init(MX_RS485_2_UARTx_Rx_GPIO, &GPIO_InitStruct);
    
  /* NVIC中设置串口中断优先级*/
  HAL_NVIC_SetPriority(MX_RS485_2_UARTx_IRQn, 3, 0);
	/* NVIC中使能串口中断*/
  HAL_NVIC_EnableIRQ(MX_RS485_2_UARTx_IRQn);
}

/**
  * 函数功能: 板载串口通信初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口通信功能
  */
void MX_RS485_2_UARTx_Init(void)
{
  /* 串口通信GPIO初始化 */
  YS_RS485_2_UARTx_MspInit();
  /* 串口通信功能配置 */
  huartx_RS485_2.Instance = MX_RS485_2_UARTx;                                   // 串口号
  huartx_RS485_2.Init.BaudRate   = MX_RS485_2_UARTx_BAUDRATE;                   // 波特率
  huartx_RS485_2.Init.WordLength = UART_WORDLENGTH_8B;                   // 有效数据长度
  huartx_RS485_2.Init.StopBits   = UART_STOPBITS_1;                      // 停止位
  huartx_RS485_2.Init.Parity     = UART_PARITY_NONE;                     // 奇偶校验
  huartx_RS485_2.Init.Mode       = UART_MODE_TX_RX;                      // 发送&接收模式
  huartx_RS485_2.Init.HwFlowCtl  = UART_HWCONTROL_NONE;                  // 硬件流控制
  huartx_RS485_2.Init.OverSampling    = UART_OVERSAMPLING_16;            // 过采样
  huartx_RS485_2.Init.OneBitSampling  = UART_ONE_BIT_SAMPLE_DISABLE;     // 采样位
  huartx_RS485_2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;  // 高级应用功能
  HAL_UART_Init(&huartx_RS485_2);
}
/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
