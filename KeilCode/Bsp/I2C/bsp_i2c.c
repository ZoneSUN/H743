/** @file bsp_i2c.c
 * @brief STM32H743 本板 I2C2 配置；移植时集中修改此文件。
 * Copyright (c) 2020-2032, MZXQ. Based on original myiic.c.
 */
#include "bsp_i2c.h"
/** 确认当前 PCLK1 和 I2C2 内核时钟均为 100 MHz。
 * TIMINGR 按此频率计算，时钟改变后拒绝继续使用旧时序。 */
static uint8_t i2c2_clock_valid(void)
{
    return HAL_RCC_GetPCLK1Freq() == 100000000U &&
        HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C2) == 100000000U;
}
/** 配置本板 I2C2 时钟来源，检查频率，再启用 GPIOH/I2C2 时钟。
 * PH4/PH5 配置 AF4 开漏，由硬件产生通信波形；板上需有外部上拉。
 * 新增总线可参考此回调，更换外设/引脚/时钟，通用读写代码无需复制。 */
static HAL_StatusTypeDef i2c2_prepare(void)
{
    RCC_PeriphCLKInitTypeDef clock = {0};
    GPIO_InitTypeDef gpio = {0};
    HAL_StatusTypeDef status;
    /* I2C1/2/3 共享时钟选择域，添加新总线时需统一规划来源，避免互相改变时钟。 */
    clock.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    clock.I2c123ClockSelection = RCC_I2C2CLKSOURCE_D2PCLK1;
    status = HAL_RCCEx_PeriphCLKConfig(&clock);
    if (status != HAL_OK) return status;
    if (!i2c2_clock_valid()) return HAL_ERROR;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_I2C2_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
    gpio.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_AF_OD; /* 复用开漏，波形由硬件外设产生。 */
    gpio.Pull = GPIO_NOPULL;    /* 使用板载 5.1 kOhm 上拉，不启用内部上拉。 */
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &gpio);
    return HAL_OK;
}
/* 100 MHz 内核时钟下的约 100 kHz 标准模式时序。
 * PRESC=9、SCLDEL=13、SDADEL=3、SCLH=44、SCLL=54。
 * 假设上升时间 <=1000 ns、下降时间 <=300 ns，实际频率需示波器确认。
 * 更换系统时钟、上拉或线路负载时需重新计算，不能沿用这个常量。 */
static const bsp_i2c_config_t i2c2_config = {
    I2C2,                       /* instance：本板所用硬件外设。 */
    0x90D32C36U,                /* timing：与上面 100 MHz 时钟匹配的 TIMINGR。 */
    I2C_ANALOGFILTER_ENABLE,    /* analog_filter：启用模拟滤波抑制短脉冲。 */
    0U,                        /* digital_filter：不增加数字滤波采样延迟。 */
    i2c2_prepare,              /* prepare：通用初始化调用此函数准备硬件。 */
    i2c2_clock_valid           /* clock_valid：初始化和传输前检查时钟。 */
};
/* 全局对象只保存本条总线的状态。HAL 句柄和 initialized 从零开始，
 * 初始化状态设为 HAL_ERROR，避免未调用初始化时误判为成功。
 * 创建第二条总线时提供另一个配置和对象，不修改此对象的配置指针。 */
bsp_i2c_bus_t g_bsp_i2c2 = {
    &i2c2_config, {0}, HAL_ERROR, HAL_ERROR, HAL_I2C_ERROR_NONE, 0U
};
