/** @file bsp_i2c.h
 * @brief 本板总线实例声明。通用对象/接口定义在 bsp_IICx_demo.h。
 * 增加总线时，在本板模块新增配置与独立对象，不复制通用 HAL 读写函数。
 */
#ifndef BSP_I2C_H
#define BSP_I2C_H
#include "bsp_IICx_demo.h"
/* 通用操作示例：bsp_i2c_init(&g_bsp_i2c2)。
 * EEPROM 层将 &g_bsp_i2c2 填入器件对象的 bus 字段即可复用该总线。
 * 扩展 I2C1 时需在 c 文件新增配置、回调和 g_bsp_i2c1，并在这里声明。
 * 同一物理外设不要创建多个总线对象，避免多个 HAL 句柄竞争硬件。 */
extern bsp_i2c_bus_t g_bsp_i2c2; /* PH4=SCL、PH5=SDA，板载外部上拉。 */
#endif
