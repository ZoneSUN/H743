/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: MZXQ嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2018-09-09
  * 功    能: FDCAN-API
  ******************************************************************************
  * 说明：
  * 本例程配套MZXQstm32开发板使用。
  * 
  * 淘宝：
  * 论坛：MZXQ
  * 版权归MZXQ嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include <stdio.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define FDCANx                          FDCAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_FDCAN_CLK_ENABLE()

#define CANx_GPIO_CLK_ENABLE()          {__HAL_RCC_GPIOB_CLK_ENABLE();}
#define CANx_TX_GPIO_PORT               GPIOB
#define CANx_TX_PIN                     GPIO_PIN_9

#define CANx_RX_GPIO_PORT               GPIOB
#define CANx_RX_PIN                     GPIO_PIN_8

#define GPIO_AFx_CAN                    GPIO_AF9_FDCAN1

#define FDCANx_IT0_IRQn                 FDCAN1_IT0_IRQn
#define FDCANx_IT0_IRQHandler           FDCAN1_IT0_IRQHandler


#define FDCANy                          FDCAN2
#define CANy_CLK_ENABLE()               __HAL_RCC_FDCAN_CLK_ENABLE()

#define CANy_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define CANy_TX_GPIO_PORT               GPIOB
#define CANy_TX_PIN                     GPIO_PIN_6

#define CANy_RX_GPIO_PORT               GPIOB
#define CANy_RX_PIN                     GPIO_PIN_5

#define GPIO_AFy_CAN                    GPIO_AF9_FDCAN2

#define FDCANy_IT0_IRQn                 FDCAN2_IT0_IRQn
#define FDCANy_IT0_IRQHandler           FDCAN2_IT0_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
extern FDCAN_HandleTypeDef hFDCanx;
extern FDCAN_RxHeaderTypeDef FDCANx_Rx;
extern FDCAN_TxHeaderTypeDef FDCANx_Tx;

extern FDCAN_HandleTypeDef hFDCany;
extern FDCAN_RxHeaderTypeDef FDCANy_Rx;
extern FDCAN_TxHeaderTypeDef FDCANy_Tx;
/* 函数声明 ------------------------------------------------------------------*/
void MX_CANx_Init(void);
void MX_CANy_Init(void);
uint8_t FDCANx_Send_Msg(uint8_t* msg);

uint8_t FDCANy_Send_Msg(uint8_t* msg);

#endif /* __BSP_CAN_H__ */


/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
