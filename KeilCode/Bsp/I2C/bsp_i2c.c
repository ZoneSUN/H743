/**
 * @file bsp_i2c.c
 * @brief 选择总线及具体时序，调用通用初始化，不引入配置对象。
 */
#include "bsp_i2c.h"

/**
 * @brief 将本板选定的五个时序字段拼接为TIMINGR，供公共初始化使用。
 * @return 当前配置返回0x90D32C36，与原初始化数值一致。
 * @note 参数基于100MHz内核时钟及原线路条件：计数单位为
 *       (PRESC+1)/100MHz=100ns，SCLL计数5.5us，SCLH计数4.5us，
 *       SCLDEL计数1.4us，SDADEL计数0.3us。
 *       实际总线周期还包含同步、滤波和线路上升下降时间。
 *       此函数只拼接已选定的时序，不自动适配不同系统时钟或总线速度。
 */
static uint32_t I2C_Timing_Calculate(void)
{
    const uint32_t presc = 9U;    /* 4位：预分频实际为PRESC+1。 */
    const uint32_t scldel = 13U; /* 4位：数据建立延时计数。 */
    const uint32_t sdadel = 3U;  /* 4位：数据保持延时计数。 */
    const uint32_t sclh = 44U;   /* 8位：SCL高电平计数，实际为SCLH+1。 */
    const uint32_t scll = 54U;   /* 8位：SCL低电平计数，实际为SCLL+1。 */

    return (presc << I2C_TIMINGR_PRESC_Pos) |
           (scldel << I2C_TIMINGR_SCLDEL_Pos) |
           (sdadel << I2C_TIMINGR_SDADEL_Pos) |
           (sclh << I2C_TIMINGR_SCLH_Pos) |
           (scll << I2C_TIMINGR_SCLL_Pos);
}

/* I2C1：PB8=SCL、PB9=SDA，使用独立hi2c1句柄。
 * Timing按100MHz内核时钟及原线路条件计算，实际SCL频率需测量确认。
 * 时钟源在MSP配置，GPIO参数在此传入；时钟变化时应重新计算此参数。 */
void bsp_I2C1_Start(void)
{
    /* 先开启本总线的GPIO与外设时钟，随后公共初始化才可访问寄存器。 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    MX_I2C_Init(&hi2c1, I2C1, I2C_Timing_Calculate(),
                GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_AF4_I2C1);
}

/* I2C2：PH4=SCL、PH5=SDA，当前24C02使用这条总线。
 * 与I2C1采用相同100MHz内核时钟约定，两个外设不共用HAL运行状态。 */
void bsp_I2C2_Start(void)
{
    /* 仅使能GPIOH和I2C2；调用此函数不会开启I2C1。 */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_I2C2_CLK_ENABLE();
    MX_I2C_Init(&hi2c2, I2C2, I2C_Timing_Calculate(),
                GPIOH, GPIO_PIN_4 | GPIO_PIN_5, GPIO_AF4_I2C2);
}


