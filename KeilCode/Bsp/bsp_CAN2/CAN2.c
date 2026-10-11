/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:23:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:23:00
 * @Description: CAN2 BSP，对应开发板 FDCAN2
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#include "CAN2.h"

FDCAN_HandleTypeDef can2_handle;
FDCAN_RxHeaderTypeDef can2_rx_header;
uint8_t can2_rx_buf[CAN2_RX_BUFFER_SIZE];

void CAN2_Start(uint32_t baudrate)
{
  HAL_StatusTypeDef clk_status = CANFDx_PeriphCLK_Config(FDCAN2, RCC_FDCANCLKSOURCE_HSE);
  printf("CAN2_Start clock status: %d\r\n", (int)clk_status);

  /* PB6 TX / PB5 RX */
  HAL_StatusTypeDef gpio_status = CANFDx_GPIO_Init(GPIOB, GPIOB,
                                                   GPIO_PIN_6, GPIO_PIN_5,
                                                   GPIO_AF9_FDCAN2, GPIO_AF9_FDCAN2);
  CANFDx_CLK_Init(FDCAN2);
  HAL_StatusTypeDef can_status = CANFDx_Init(&can2_handle, FDCAN2, baudrate);
  (void)gpio_status;
  (void)can_status;

  /* 掩码为 0：接收全部标准帧 */
  (void)CANFDx_FilterConfig(&can2_handle, 0x000U, 0x000U);
  CANFDx_NVIC_Init(FDCAN2_IT0_IRQn, 5, 0);
  (void)CANFDx_Start(&can2_handle);
}

HAL_StatusTypeDef CAN2_Send(uint32_t id, uint32_t data0, uint32_t data1, uint32_t data2)
{
  FDCAN_TxHeaderTypeDef tx = {0};
  uint8_t msg[4];

  msg[0] = (uint8_t)id;
  msg[1] = (uint8_t)data0;
  msg[2] = (uint8_t)data1;
  msg[3] = (uint8_t)data2;

  tx.Identifier = id & 0x7FFU;
  tx.IdType = FDCAN_STANDARD_ID;
  tx.TxFrameType = FDCAN_DATA_FRAME;
  tx.DataLength = FDCAN_DLC_BYTES_4;
  tx.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  tx.BitRateSwitch = FDCAN_BRS_OFF;
  tx.FDFormat = FDCAN_CLASSIC_CAN;
  tx.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  tx.MessageMarker = 0U;
  return HAL_FDCAN_AddMessageToTxFifoQ(&can2_handle, &tx, msg);
}

void FDCAN2_IT0_IRQHandler(void)
{
  HAL_FDCAN_IRQHandler(&can2_handle);
}
