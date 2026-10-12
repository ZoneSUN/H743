/**
 * @file bsp_i2c.h
 * @brief 板级启动入口。底层参数和GPIO配置统一在bsp_IICx_demo中设置。
 */
#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "bsp_IICx_demo.h"

/* 在HAL_Init和系统时钟初始化后调用；当前24C02使用I2C2。 */
void bsp_I2C1_Start(void); /* PB8=SCL、PB9=SDA。 */
void bsp_I2C2_Start(void); /* PH4=SCL、PH5=SDA。 */

#endif
