/**
  ******************************************************************************
  * 文件名程: bsp_debug_usart.c 
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
#include "bsp_debug_usart.h"
#include <stdio.h>

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
UART_HandleTypeDef husart_debug;

/* 扩展变量 ------------------------------------------------------------------*/

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载串口引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口引脚
  */
static void DEBUG_UARTx_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};

  /* USART 时钟使能 */
  DEBUG_USART_RCC_CLK_ENABLE();
  /* 引脚 时钟使能 */
  DEBUG_USARTx_Tx_GPIO_ClK_ENABLE();
	DEBUG_USARTx_Rx_GPIO_ClK_ENABLE();
  
  /* USART GPIO 配置: 复用功能模式  */
  GPIO_InitStruct.Pin = DEBUG_USARTx_Tx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USARTx_GPIO_AFx;
  HAL_GPIO_Init(DEBUG_USARTx_Tx_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = DEBUG_USARTx_Rx_GPIO_PIN;
  HAL_GPIO_Init(DEBUG_USARTx_Rx_GPIO, &GPIO_InitStruct);

}

/**
  * 函数功能: 板载串口通信初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 初始化串口通信功能
  */
void DEBUG_USARTx_Init(void)
{
  /* 串口通信GPIO初始化 */
  DEBUG_UARTx_MspInit();
  /* 串口通信功能配置 */
  husart_debug.Instance = DEBUG_USARTx;                                   // 串口号
  husart_debug.Init.BaudRate   = DEBUG_USARTx_BAUDRATE;                   // 波特率
  husart_debug.Init.WordLength = UART_WORDLENGTH_8B;                   // 有效数据长度
  husart_debug.Init.StopBits   = UART_STOPBITS_1;                      // 停止位
  husart_debug.Init.Parity     = UART_PARITY_NONE;                     // 奇偶校验
  husart_debug.Init.Mode       = UART_MODE_TX_RX;                      // 发送&接收模式
  husart_debug.Init.HwFlowCtl  = UART_HWCONTROL_NONE;                  // 硬件流控制
  husart_debug.Init.OverSampling    = UART_OVERSAMPLING_16;            // 过采样
  husart_debug.Init.OneBitSampling  = UART_ONE_BIT_SAMPLE_DISABLE;     // 采样位
  husart_debug.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;  // 高级应用功能
  HAL_UART_Init(&husart_debug);
}


/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *stream)
{
  HAL_UART_Transmit(&husart_debug, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *stream)
{
  uint8_t ch = 0;
  while(HAL_UART_Receive(&husart_debug,&ch, 1, 0xffff)!=HAL_OK);
  return ch;
}

/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
