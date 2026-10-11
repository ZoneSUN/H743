/*
 * @Author: yangwuyang
 * @Date: 2026-09-28 10:04:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-09-29 09:07:00
 * @Description: USARTx interrupt callback
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#include "bsp_USARTx_irq.h"
#include "USART2_485.h"
#include "UART7_485.h"
#include "USART1_485.h"
#include "UART4_485.h"
#include "BRT38Encoder_Process.h"
__IO uint8_t  USARTx_TxHalfCpltFlag = 0;
__IO uint8_t  USARTx_TxCpltFlag = 0;
__IO uint8_t  USARTx_RxHalfCpltFlag = 0;
__IO uint8_t  USARTx_RxCpltFlag = 0;
__IO uint8_t  USARTx_AbortCpltFlag = 0;
__IO uint32_t USARTx_ErrorCode = 0;

static uint8_t USARTx_IsTarget(UART_HandleTypeDef *huart)
{
  return ((huart->Instance == USART1) || (huart->Instance == USART2) ||
          (huart->Instance == UART4) || (huart->Instance == UART7)) ? 1U : 0U;
}

/**
 * @name HAL_USART_TxHalfCpltCallback
 * @brief 发送一半完成回调函数 HAL_UART_Transmit_IT 中断发送
 * @param husart USART句柄
 * @return void
 * @note 这个回调函数是在USART发送一半完成时调用
 */
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  if (USARTx_IsTarget(huart) != 0U)
  {
    USARTx_TxHalfCpltFlag = 1U;
  }
}
/**
 * @name HAL_UARTEx_RxEventCallback
 * @brief ToIdle 接收事件：IDLE 停顿、缓冲收满(TC)、DMA 半满(HT)
 * @param huart UART句柄
 * @param Size 本次有效字节数，不是事件枚举
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  uint16_t i;
  const char *evt = "";

  if (huart->RxEventType == HAL_UART_RXEVENT_IDLE)//空闲中断
  {
    evt = "IDLE";
    if (huart->Instance == UART7)//UART7中断
    {
     uint16_t n = Size;
     uint8_t snap[UART7_485_RX_BUFFER_SIZE];
     if (n > UART7_485_RX_BUFFER_SIZE)
     {
       n = UART7_485_RX_BUFFER_SIZE;
     }
     for (i = 0U; i < n; i++)
     {
       snap[i] = uart7_rx_buf[i];
     }
     BRT38_EncoderPacket_Update(snap, n);
     if (huart->hdmarx != NULL)
     {
      (void)USARTx_ReceiveToIdle_DMA(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
     }
     else
     {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
     }
     BRT38_Job_Post();
   }
   if (huart->Instance == USART2)
   {
     uint16_t n = Size;
     uint8_t snap[USART2_485_RX_BUFFER_SIZE];
     if (n > USART2_485_RX_BUFFER_SIZE)
     {
       n = USART2_485_RX_BUFFER_SIZE;
     }
     for (i = 0U; i < n; i++)
     {
       snap[i] = usart2_rx_buf[i];
     }
     if (huart->hdmarx != NULL)
     {
      (void)USARTx_ReceiveToIdle_DMA(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
     }
     else
     {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
     }
     printf("USART2 %s %u:", evt, (unsigned)n);
     for (i = 0U; i < n; i++)
     {
       printf(" %02X", snap[i]);
     }
     printf("\r\n");
   }
   if (huart->Instance == USART1)
   {
     if (huart->hdmarx != NULL)
     {
       (void)USARTx_ReceiveToIdle_DMA(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
     }
     else
     {
       (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
     }
   }
   if (huart->Instance == UART4)
   {
     if (huart->hdmarx != NULL)
     {
       (void)USARTx_ReceiveToIdle_DMA(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
     }
     else
     {
       (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
     }
   }
  }
  else if (huart->RxEventType == HAL_UART_RXEVENT_HT)//DMA半满中断
  {
    evt = "HT";
    printf("USARTx %s %u:", evt, (unsigned)Size);
    for (i = 0U; i < Size; i++)
    {
      printf(" %02X", usart2_rx_buf[i]);
    }
    printf("\r\n");
  }
  else if (huart->RxEventType == HAL_UART_RXEVENT_TC)//传输完成中断
  {
    evt = "TC";
    if (huart->Instance == USART2)
    {
      printf("USART2 %s %u:", evt, (unsigned)Size);
     for (i = 0U; i < Size; i++)
     {
       printf(" %02X", usart2_rx_buf[i]);
     }
     printf("\r\n");
     if (huart->hdmarx != NULL)
     {
      (void)USARTx_ReceiveToIdle_DMA(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
     }
     else
     {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
     }
    }
    else if (huart->Instance == UART7)
    {
      printf("UART7 %s %u:", evt, (unsigned)Size);
      for (i = 0U; i < Size; i++)
      {
        printf(" %02X", uart7_rx_buf[i]);
      }
      printf("\r\n");
      if (huart->hdmarx != NULL)
      {
        (void)USARTx_ReceiveToIdle_DMA(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
      }
      else
      {
        (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
      }
    }
    else if (huart->Instance == USART1)
    {
      if (huart->hdmarx != NULL)
      {
        (void)USARTx_ReceiveToIdle_DMA(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
      }
      else
      {
        (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
      }
    }
    else if (huart->Instance == UART4)
    {
      if (huart->hdmarx != NULL)
      {
        (void)USARTx_ReceiveToIdle_DMA(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
      }
      else
      {
        (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
      }
    }
  }
 
}
/** 
 * @name HAL_USART_TxCpltCallback
 * @brief 发送完成回调函数 HAL_UART_Transmit_IT 中断发送
 * @param husart USART句柄
 * @return void
 * @note 这个回调函数是在USART发送满 设定 字节数 完成时调用
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (USARTx_IsTarget(huart) != 0U)
  {
    USARTx_TxCpltFlag = 1U;
  }
}
/**
 * @name HAL_USART_RxHalfCpltCallback
 * @brief 接收一半完成回调函数  
 * @param husart 
 * @return void
 * @note 这个回调函数是在USART接收一半完成时调用
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  if (USARTx_IsTarget(huart) != 0U)
  {
    USARTx_RxHalfCpltFlag = 1U;
  }
}
/**
 * @name HAL_USART_RxCpltCallback
 * @brief 接收满 设定 字节数 完成时调用
 * @param husart USART句柄
 * @return void
 * @note 这个回调函数是在USART接收满 设定 字节数 完成时调用
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  printf("__func__: %s\r\n", __func__);

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  USARTx_ErrorCode = huart->ErrorCode;
  if (huart->Instance == USART2)
  {
    if (huart->hdmarx != NULL)
    {
      (void)USARTx_ReceiveToIdle_DMA(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    }
    else
    {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    }
  }
  else if (huart->Instance == UART7)
  {
    if (huart->hdmarx != NULL)
    {
      (void)USARTx_ReceiveToIdle_DMA(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
    }
    else
    {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart7_rx_buf, UART7_485_RX_BUFFER_SIZE);
    }
  }
  else if (huart->Instance == USART1)
  {
    if (huart->hdmarx != NULL)
    {
      (void)USARTx_ReceiveToIdle_DMA(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
    }
    else
    {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
    }
  }
  else if (huart->Instance == UART4)
  {
    if (huart->hdmarx != NULL)
    {
      (void)USARTx_ReceiveToIdle_DMA(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
    }
    else
    {
      (void)HAL_UARTEx_ReceiveToIdle_IT(huart, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
    }
  }
}

/**
 * @name HAL_USART_AbortCpltCallback
 * @brief HAL_UART_Abort_IT 把收发都停掉之后, 才会调用这个回调函数
 * @param husart 
 * @return void
 * @note 这个回调函数是在HAL_UART_Abort_IT 把收发都停掉之后, 才会调用这个回调函数
 * @note HAL_UART_Abort_IT 是用来停止USART的收发
 */
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
  if (USARTx_IsTarget(huart) != 0U)
  {
    USARTx_AbortCpltFlag = 1U;
  }
}
