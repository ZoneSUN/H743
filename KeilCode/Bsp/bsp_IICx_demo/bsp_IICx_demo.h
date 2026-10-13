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
 * timing、GPIO端口、同端口SCL/SDA引脚和AF由调用层传入。
 * 启动函数须先开启对应GPIO与I2C外设时钟；MSP只选择共享时钟源。
 * 本函数负责GPIO及外设参数；不读写从设备。 */
void MX_I2C_Init(I2C_HandleTypeDef *handle, I2C_TypeDef *instance, uint32_t timing,
                 GPIO_TypeDef *port, uint32_t pins, uint32_t alternate);

#endif
