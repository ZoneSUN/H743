/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:15:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:15:00
 * @Description: FDCAN BSP demo
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#include "bsp_CANFDx.h"
#include "bsp_USARTx.h"

HAL_StatusTypeDef CANFDx_PeriphCLK_Config(FDCAN_GlobalTypeDef *FDCANx, uint32_t clk_source)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit = {0};

  if ((FDCANx != FDCAN1) && (FDCANx != FDCAN2))
  {
    return HAL_ERROR;
  }

  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
  RCC_PeriphClkInit.FdcanClockSelection = clk_source;
  return HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
}

void CANFDx_CLK_Init(FDCAN_GlobalTypeDef *FDCANx)
{
  if ((FDCANx != FDCAN1) && (FDCANx != FDCAN2))
  {
    return;
  }
  __HAL_RCC_FDCAN_CLK_ENABLE();
}

HAL_StatusTypeDef CANFDx_GPIO_Init(GPIO_TypeDef *GPIOx_tx,
                                   GPIO_TypeDef *GPIOx_rx,
                                   uint32_t tx_pin,
                                   uint32_t rx_pin,
                                   uint32_t tx_af,
                                   uint32_t rx_af)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if ((GPIOx_tx == NULL) || (GPIOx_rx == NULL))
  {
    return HAL_ERROR;
  }

  GPIOx_CLK_Init(GPIOx_tx);
  GPIOx_CLK_Init(GPIOx_rx);

  GPIO_InitStruct.Pin = tx_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = tx_af;
  HAL_GPIO_Init(GPIOx_tx, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = rx_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = rx_af;
  HAL_GPIO_Init(GPIOx_rx, &GPIO_InitStruct);

  return HAL_OK;
}

static uint8_t CANFDx_FindTiming(uint32_t clk_hz, uint32_t baud,
                                 uint32_t prescaler_max, uint32_t seg1_max, uint32_t seg2_max,
                                 uint32_t *prescaler, uint32_t *seg1, uint32_t *seg2, uint32_t *sjw)
{
  uint32_t p;
  uint32_t ticks;

  if ((baud == 0U) || (clk_hz < baud) || ((clk_hz % baud) != 0U))
  {
    return 0U;
  }

  ticks = clk_hz / baud;
  for (p = 1U; p <= prescaler_max; p++)
  {
    uint32_t nq;
    uint32_t s1;
    uint32_t s2;
    uint32_t jump;

    if ((ticks % p) != 0U)
    {
      continue;
    }
    nq = ticks / p;
    if ((nq < 8U) || (nq > (1U + seg1_max + seg2_max)))
    {
      continue;
    }
    s1 = ((nq * 80U) / 100U) - 1U;
    if (s1 < 2U)
    {
      s1 = 2U;
    }
    if (s1 > seg1_max)
    {
      continue;
    }
    s2 = nq - 1U - s1;
    if ((s2 < 2U) || (s2 > seg2_max))
    {
      continue;
    }
    jump = (s2 > 4U) ? 4U : s2;
    *prescaler = p;
    *seg1 = s1;
    *seg2 = s2;
    *sjw = jump;
    return 1U;
  }
  return 0U;
}

HAL_StatusTypeDef CANFDx_Init(FDCAN_HandleTypeDef *hfdcan,
                              FDCAN_GlobalTypeDef *FDCANx,
                              uint32_t BaudRate)
{
  uint32_t clk_hz;
  uint32_t nominal_prescaler;
  uint32_t nominal_seg1;
  uint32_t nominal_seg2;
  uint32_t nominal_sjw;
  uint32_t data_prescaler;
  uint32_t data_seg1;
  uint32_t data_seg2;
  uint32_t data_sjw;
  uint32_t frame_format = FDCAN_FRAME_FD_BRS;

  if ((hfdcan == NULL) || ((FDCANx != FDCAN1) && (FDCANx != FDCAN2)) || (BaudRate == 0U))
  {
    return HAL_ERROR;
  }

  clk_hz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN);
  if (CANFDx_FindTiming(clk_hz, BaudRate, 512U, 256U, 128U,
                        &nominal_prescaler, &nominal_seg1, &nominal_seg2, &nominal_sjw) == 0U)
  {
    return HAL_ERROR;
  }
  if (CANFDx_FindTiming(clk_hz, BaudRate, 32U, 32U, 16U,
                        &data_prescaler, &data_seg1, &data_seg2, &data_sjw) == 0U)
  {
    frame_format = FDCAN_FRAME_CLASSIC;
    data_prescaler = 1U;
    data_seg1 = 2U;
    data_seg2 = 2U;
    data_sjw = 1U;
  }

  hfdcan->Instance = FDCANx;//FDCAN1 或 FDCAN2 外设名称
  hfdcan->Init.FrameFormat = frame_format;//FDCAN_FRAME_FD_BRS 或 FDCAN_FRAME_CLASSIC 帧格式
  hfdcan->Init.Mode = FDCAN_MODE_NORMAL;//FDCAN_MODE_NORMAL 正常模式
  hfdcan->Init.AutoRetransmission = DISABLE;//FDCAN_AUTORETRANSMISSION_DISABLE 自动重传关闭
  hfdcan->Init.TransmitPause = DISABLE;//FDCAN_TRANSMITPAUSE_DISABLE 发送暂停关闭
  hfdcan->Init.ProtocolException = DISABLE;//FDCAN_PROTOCOLEXCEPTION_DISABLE 协议异常关闭
  hfdcan->Init.NominalPrescaler = nominal_prescaler;//名义段分频
  hfdcan->Init.NominalSyncJumpWidth = nominal_sjw;//名义段同步跳宽
  hfdcan->Init.NominalTimeSeg1 = nominal_seg1;//名义段 1 时间份额
  hfdcan->Init.NominalTimeSeg2 = nominal_seg2;//名义段 2 时间份额
  hfdcan->Init.DataPrescaler = data_prescaler;//数据段分频
  hfdcan->Init.DataSyncJumpWidth = data_sjw;//数据段同步跳宽
  hfdcan->Init.DataTimeSeg1 = data_seg1;//数据段 1 时间份额
  hfdcan->Init.DataTimeSeg2 = data_seg2;//数据段 2 时间份额
  /* FDCAN1、FDCAN2 共用消息 RAM。FDCAN1 占用约 65 个字，FDCAN2 从 128 起 */
  hfdcan->Init.MessageRAMOffset = (FDCANx == FDCAN2) ? 128U : 0U;
  hfdcan->Init.StdFiltersNbr = 1;//标准过滤器数量
  hfdcan->Init.ExtFiltersNbr = 0;//扩展过滤器数量
  hfdcan->Init.RxFifo0ElmtsNbr = 8;//RX FIFO0 元素数量
  hfdcan->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;//RX FIFO0 元素大小
  hfdcan->Init.RxFifo1ElmtsNbr = 0;//RX FIFO1 元素数量
  hfdcan->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;//RX FIFO1 元素大小
  hfdcan->Init.RxBuffersNbr = 0;//RX 缓冲区数量
  hfdcan->Init.RxBufferSize = FDCAN_DATA_BYTES_8;//RX 缓冲区大小
  hfdcan->Init.TxEventsNbr = 0;//TX 事件数量
  hfdcan->Init.TxBuffersNbr = 0;//TX 缓冲区数量
  hfdcan->Init.TxFifoQueueElmtsNbr = 8;//TX FIFO 队列元素数量
  hfdcan->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;//TX FIFO 队列模式
  hfdcan->Init.TxElmtSize = FDCAN_DATA_BYTES_8;//TX 元素大小

  return HAL_FDCAN_Init(hfdcan);
}

HAL_StatusTypeDef CANFDx_FilterConfig(FDCAN_HandleTypeDef *hfdcan,
                                      uint32_t filter_id,
                                      uint32_t filter_mask)
{
  FDCAN_FilterTypeDef filter = {0};

  if (hfdcan == NULL)
  {
    return HAL_ERROR;
  }

  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = filter_id;
  filter.FilterID2 = filter_mask;
  return HAL_FDCAN_ConfigFilter(hfdcan, &filter);
}

HAL_StatusTypeDef CANFDx_Start(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status;

  if (hfdcan == NULL)
  {
    return HAL_ERROR;
  }

  status = HAL_FDCAN_Start(hfdcan);
  if (status != HAL_OK)
  {
    return status;
  }
  return HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

void CANFDx_NVIC_Init(IRQn_Type irq, uint32_t preemptionPriority, uint32_t subPriority)
{
  HAL_NVIC_SetPriority(irq, preemptionPriority, subPriority);
  HAL_NVIC_EnableIRQ(irq);
}
