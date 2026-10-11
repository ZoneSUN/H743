/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:23:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:23:00
 * @Description: CAN1 BSP，对应开发板 FDCAN1
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#include "CAN1.h"
#include "CAN2.h"

FDCAN_HandleTypeDef can1_handle;
FDCAN_RxHeaderTypeDef can1_rx_header;
uint8_t can1_rx_buf[CAN1_RX_BUFFER_SIZE];

void CAN1_Start(uint32_t baudrate)
{
  HAL_StatusTypeDef clk_status = CANFDx_PeriphCLK_Config(FDCAN1, RCC_FDCANCLKSOURCE_HSE);
  printf("CAN1_Start clock status: %d\r\n", (int)clk_status);

  /* PB9 TX / PB8 RX */
  HAL_StatusTypeDef gpio_status = CANFDx_GPIO_Init(GPIOB, GPIOB,
                                                   GPIO_PIN_9, GPIO_PIN_8,
                                                   GPIO_AF9_FDCAN1, GPIO_AF9_FDCAN1);
  CANFDx_CLK_Init(FDCAN1);
  HAL_StatusTypeDef can_status = CANFDx_Init(&can1_handle, FDCAN1, baudrate);
  (void)gpio_status;
  (void)can_status;

  /* 掩码为 0：接收全部标准帧 */
  (void)CANFDx_FilterConfig(&can1_handle, 0x000U, 0x000U);
  CANFDx_NVIC_Init(FDCAN1_IT0_IRQn, 5, 0);
  (void)CANFDx_Start(&can1_handle);
}

void FDCAN1_IT0_IRQHandler(void)
{
  HAL_FDCAN_IRQHandler(&can1_handle);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U)
  {
    return;
  }

  if (hfdcan->Instance == FDCAN1)
  {
    (void)HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &can1_rx_header, can1_rx_buf);
  }
  else if (hfdcan->Instance == FDCAN2)
  {
    (void)HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &can2_rx_header, can2_rx_buf);
  }
}
