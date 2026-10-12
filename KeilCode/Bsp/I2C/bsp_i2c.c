/**
 * @file bsp_i2c.c
 * @brief 选择总线及具体时序，调用通用初始化，不引入配置对象。
 */
#include "bsp_i2c.h"

/* I2C1：PB8=SCL、PB9=SDA，使用独立hi2c1句柄。
 * Timing按100MHz内核时钟及原线路条件计算，实际SCL频率需测量确认。
 * 时钟源和GPIO仍统一在MSP配置；时钟变化时应重新计算此参数。 */
void bsp_I2C1_Start(void)
{
    MX_I2C_Init(&hi2c1, I2C1, 0x90D32C36U);
}

/* I2C2：PH4=SCL、PH5=SDA，当前24C02使用这条总线。
 * 与I2C1采用相同100MHz内核时钟约定，两个外设不共用HAL运行状态。 */
void bsp_I2C2_Start(void)
{
    MX_I2C_Init(&hi2c2, I2C2, 0x90D32C36U);
}


