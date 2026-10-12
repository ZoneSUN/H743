/**
 * @file bsp_IICx_demo.h
 * @brief 两条总线的独立HAL句柄及通用初始化声明。
 */
#ifndef BSP_IICX_DEMO_H
#define BSP_IICX_DEMO_H

#include "stm32h7xx_hal.h"

/* 每个硬件外设保留独立句柄，保存各自HAL运行状态。 */
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

/* HAL和系统时钟初始化后调用；handle、instance必须匹配，
 * timing由调用层设置。只初始化外设，不读写从设备。 */
void MX_I2C_Init(I2C_HandleTypeDef *handle, I2C_TypeDef *instance, uint32_t timing);

#endif
