/*
 * @Author: yangwuyang
 * @Date: 2026-10-07 18:15:00
 * @LastEditors: yangwuyang
 * @LastEditTime: 2026-10-07 18:15:00
 * @Description: FDCAN BSP demo
 *
 * Copyright (c) 2026 by yangwuyang, All Rights Reserved.
 */
#ifndef __BSP_CANFDX_H__
#define __BSP_CANFDX_H__

#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"

/**
 * @brief 配置 FDCAN 内核时钟源。FDCAN1 与 FDCAN2 共用这一路时钟
 * @param FDCANx: FDCAN1 或 FDCAN2
 * @param clk_source: RCC_FDCANCLKSOURCE_HSE、RCC_FDCANCLKSOURCE_PLL、RCC_FDCANCLKSOURCE_PLL2
 * @return HAL_StatusTypeDef
 * @note inline 作用：在编译时，将函数替换为函数体，而不是调用函数。可以提高执行效率。
 * @example
 * CANFDx_PeriphCLK_Config(FDCAN1, RCC_FDCANCLKSOURCE_PLL);
 */
inline HAL_StatusTypeDef CANFDx_PeriphCLK_Config(FDCAN_GlobalTypeDef *FDCANx, uint32_t clk_source);

/**
 * @brief 使能 FDCAN 内核时钟。FDCAN1 与 FDCAN2 共用
 * @param FDCANx: FDCAN1 或 FDCAN2
 * @example
 * CANFDx_CLK_Init(FDCAN1);
 */
inline void CANFDx_CLK_Init(FDCAN_GlobalTypeDef *FDCANx);

/**
 * @brief 初始化 FDCAN 的 TX/RX 引脚
 * @param GPIOx_tx: TX 引脚所在端口
 * @param GPIOx_rx: RX 引脚所在端口
 * @param tx_pin: TX 引脚
 * @param rx_pin: RX 引脚
 * @param tx_af: TX 复用，例如 GPIO_AF9_FDCAN1
 * @param rx_af: RX 复用，例如 GPIO_AF9_FDCAN1
 * @return HAL_StatusTypeDef
 * @example
 * CANFDx_GPIO_Init(GPIOB, GPIOB, GPIO_PIN_9, GPIO_PIN_8, GPIO_AF9_FDCAN1, GPIO_AF9_FDCAN1);
 */
inline HAL_StatusTypeDef CANFDx_GPIO_Init(GPIO_TypeDef *GPIOx_tx,
                                          GPIO_TypeDef *GPIOx_rx,
                                          uint32_t tx_pin,
                                          uint32_t rx_pin,
                                          uint32_t tx_af,
                                          uint32_t rx_af);

/**
 * @brief 初始化 FDCAN。按当前内核时钟把 BaudRate 换算成位时间，采样点约 80%
 * @param hfdcan: FDCAN 句柄，需长期有效
 * @param FDCANx: FDCAN1 或 FDCAN2
 * @param BaudRate: 位速率，单位 bit/s，例如 500000 表示 500kbps
 * @return HAL_StatusTypeDef
 * @note 调用前先配置内核时钟并 CANFDx_CLK_Init。时钟频率须能整除 BaudRate。数据段算不出时退回经典 CAN。FDCAN2 的消息 RAM 偏移避开 FDCAN1
 * @note 波特率计算公式：BaudRate = F_CAN_CLK / (NominalPrescaler * (BS1 + BS2 + 1))
 * @note F_CAN_CLK 为内核时钟频率，例如 100MHz
 * @note NominalPrescaler 为名义段分频，例如 2
 * @note BS1 为名义段 1 时间份额，例如 1
 * @note BS2 为名义段 2 时间份额，例如 15
 * @example
 * FDCAN_HandleTypeDef hfdcan1;
 * CANFDx_Init(&hfdcan1, FDCAN1, 100000U);
 */
inline HAL_StatusTypeDef CANFDx_Init(FDCAN_HandleTypeDef *hfdcan,
                                     FDCAN_GlobalTypeDef *FDCANx,
                                     uint32_t BaudRate);

/**
 * @brief 配置标准帧掩码滤波器，匹配的帧进入 RX FIFO0
 * @param hfdcan: 已初始化的 FDCAN 句柄
 * @param filter_id: 标准 ID，11 位
 * @param filter_mask: 掩码，位为 1 表示必须与 filter_id 相同
 * @return HAL_StatusTypeDef
 * @example
 * CANFDx_FilterConfig(&hfdcan1, 0x112U, 0x7FFU);
 */
inline HAL_StatusTypeDef CANFDx_FilterConfig(FDCAN_HandleTypeDef *hfdcan,
                                             uint32_t filter_id,
                                             uint32_t filter_mask);

/**
 * @brief 启动 FDCAN，并打开 RX FIFO0 新消息中断
 * @param hfdcan: 已完成滤波配置的句柄
 * @return HAL_StatusTypeDef
 */
inline HAL_StatusTypeDef CANFDx_Start(FDCAN_HandleTypeDef *hfdcan);

/**
 * @brief 配置并使能 FDCAN 中断
 * @param irq: 例如 FDCAN1_IT0_IRQn
 * @param preemptionPriority: 抢占优先级，0~15，0 最高。分组 4 下建议用 5
 * @param subPriority: 子优先级。分组 4 下无效
 * @example
 * CANFDx_NVIC_Init(FDCAN1_IT0_IRQn, 5, 0);
 */
inline void CANFDx_NVIC_Init(IRQn_Type irq,
                             uint32_t preemptionPriority,
                             uint32_t subPriority);

#endif /* __BSP_CANFDX_H__ */
