/**
 * @file bsp_IICx_demo.c
 * @brief 通用 HAL 阻塞访问实现；不定义全局 HAL_I2C_MspInit，避免影响其他总线。
 * 源自原 myiic.c 的硬件 I2C2 实现，现将句柄和状态移入各自总线对象。
 * Copyright (c) 2020-2032, MZXQ. Porting changes maintained in this project.
 */
#include "bsp_IICx_demo.h"

/* 每次操作覆盖最近状态，软件参数错误清除旧 HAL 错误，避免误报历史错误。 */
/** 将最近操作状态保存在当前总线对象中，不影响其他总线。
 * error 是 HAL 错误位或 NONE；软件参数错误使用 NONE 清除旧错误。 */
static HAL_StatusTypeDef record(bsp_i2c_bus_t *bus, HAL_StatusTypeDef status, uint32_t error)
{
    if (bus != NULL) { bus->last_status = status; bus->last_error = error; }
    return status;
}
/** 传输前统一检查对象、7 位器件地址、初始化标志和当前时钟。
 * 时钟失效时清除初始化状态，调用者需要恢复时钟并重新初始化。 */
static HAL_StatusTypeDef check_bus(bsp_i2c_bus_t *bus, uint16_t address)
{
    if (bus == NULL || bus->config == NULL || address > 0x7FU)
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    if (!bus->initialized)
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    if (!bus->config->clock_valid()) {
        bus->initialized = 0U;
        bus->init_status = HAL_ERROR;
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    }
    return HAL_OK;
}
/** 按总线配置调用板级准备回调，再设置 HAL 句柄和滤波。
 * bus 必须长期有效；成功置 initialized=1，失败保存 init_status。
 * 已初始化且时钟正确时复用现有配置；不检查任何器件是否应答。 */
HAL_StatusTypeDef bsp_i2c_init(bsp_i2c_bus_t *bus)
{
    HAL_StatusTypeDef status;
    if (bus == NULL) return HAL_ERROR;
    if (bus->config == NULL || bus->config->instance == NULL ||
        bus->config->prepare == NULL || bus->config->clock_valid == NULL ||
        bus->config->digital_filter > 15U ||
        (bus->config->analog_filter != I2C_ANALOGFILTER_ENABLE &&
         bus->config->analog_filter != I2C_ANALOGFILTER_DISABLE)) {
        bus->initialized = 0U; bus->init_status = HAL_ERROR;
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    }
    if (bus->initialized && bus->config->clock_valid()) {
        bus->init_status = HAL_OK;
        return record(bus, HAL_OK, HAL_I2C_ERROR_NONE);
    }
    /* 准备失败时保持不可传输状态，防止调用者沿用部分初始化的外设。 */
    bus->initialized = 0U;
    status = bus->config->prepare();
    if (status != HAL_OK || !bus->config->clock_valid()) {
        bus->init_status = (status == HAL_OK) ? HAL_ERROR : status;
        return record(bus, bus->init_status, HAL_I2C_ERROR_NONE);
    }
    bus->handle.Instance = bus->config->instance;
    bus->handle.Init.Timing = bus->config->timing;
    /* 本模块作为 7 位地址主机访问器件，不配置双地址或通用广播响应。
     * OwnAddress 是 MCU 自身的从机地址，不是 EEPROM 的 0x50 地址。 */
    bus->handle.Init.OwnAddress1 = 0U;
    bus->handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    bus->handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    bus->handle.Init.OwnAddress2 = 0U;
    bus->handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    bus->handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    bus->handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    /* 按顺序初始化外设和滤波；前一步失败就跳过后续配置。 */
    status = HAL_I2C_Init(&bus->handle);
    if (status == HAL_OK)
        status = HAL_I2CEx_ConfigAnalogFilter(&bus->handle, bus->config->analog_filter);
    if (status == HAL_OK)
        status = HAL_I2CEx_ConfigDigitalFilter(&bus->handle, bus->config->digital_filter);
    bus->init_status = status;
    bus->initialized = (status == HAL_OK) ? 1U : 0U;
    return record(bus, status, HAL_I2C_GetError(&bus->handle));
}
/* 统一校验存储地址长度；8 位地址不允许静默截断为低 8 位。 */
/** memory 是器件内部地址，size 是 HAL 的 8BIT/16BIT 地址宽度常量。
 * 8 位模式拒绝超过 0xFF 的地址，避免 HAL 静默截断。 */
static uint8_t memory_valid(uint16_t memory, uint16_t size)
{
    return (size == I2C_MEMADD_SIZE_16BIT) ||
        (size == I2C_MEMADD_SIZE_8BIT && memory <= 0xFFU);
}
/** address 为未左移 7 位器件地址，memory 为内部地址，length 为字节数。
 * timeout_ms 单位为毫秒；阻塞到完成或出错，返回 HAL 状态并记录错误码。
 * 零长度只校验不传输；非零读取失败可能部分修改 data。 */
HAL_StatusTypeDef bsp_i2c_mem_read(bsp_i2c_bus_t *bus, uint16_t address,
    uint16_t memory, uint16_t memory_size, uint8_t *data, uint16_t length, uint32_t timeout_ms)
{
    HAL_StatusTypeDef status;
    if (check_bus(bus, address) != HAL_OK) return HAL_ERROR;
    if (!memory_valid(memory, memory_size) || (data == NULL && length != 0U))
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    if (length == 0U) return record(bus, HAL_OK, HAL_I2C_ERROR_NONE);
    /* 地址左移只在 HAL 边界进行，调用者始终传 0x50 而不是 0xA0。 */
    status = HAL_I2C_Mem_Read(&bus->handle, (uint16_t)(address << 1), memory,
        memory_size, data, length, timeout_ms);
    return record(bus, status, HAL_I2C_GetError(&bus->handle));
}
/** 参数含义同读取接口，data 是只读发送缓冲区。
 * 仅完成总线传输，不负责 EEPROM 的页边界和写周期等待。
 * 返回 HAL 状态；HAL 发送接口缺少 const，转换仅用于调用旧 HAL。 */
HAL_StatusTypeDef bsp_i2c_mem_write(bsp_i2c_bus_t *bus, uint16_t address,
    uint16_t memory, uint16_t memory_size, const uint8_t *data, uint16_t length, uint32_t timeout_ms)
{
    HAL_StatusTypeDef status;
    if (check_bus(bus, address) != HAL_OK) return HAL_ERROR;
    if (!memory_valid(memory, memory_size) || (data == NULL && length != 0U))
        return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    if (length == 0U) return record(bus, HAL_OK, HAL_I2C_ERROR_NONE);
    /* HAL 老版本接口未加 const，HAL 发送过程不会修改该缓冲区。 */
    status = HAL_I2C_Mem_Write(&bus->handle, (uint16_t)(address << 1), memory,
        memory_size, (uint8_t *)data, length, timeout_ms);
    return record(bus, status, HAL_I2C_GetError(&bus->handle));
}
/** 探测 address 是否 ACK，trials 为 HAL 重试次数且不能为 0。
 * timeout_ms 为毫秒；此函数不读写存储内容，返回值不保证器件写入正确。 */
HAL_StatusTypeDef bsp_i2c_is_device_ready(bsp_i2c_bus_t *bus, uint16_t address,
    uint32_t trials, uint32_t timeout_ms)
{
    HAL_StatusTypeDef status;
    if (check_bus(bus, address) != HAL_OK) return HAL_ERROR;
    if (trials == 0U) return record(bus, HAL_ERROR, HAL_I2C_ERROR_NONE);
    status = HAL_I2C_IsDeviceReady(&bus->handle, (uint16_t)(address << 1), trials, timeout_ms);
    return record(bus, status, HAL_I2C_GetError(&bus->handle));
}
/** 返回最近初始化结果；bus=NULL 返回 HAL_ERROR。
 * 未初始化对象的 init_status 应按板级示例设为 HAL_ERROR。 */
HAL_StatusTypeDef bsp_i2c_get_init_status(const bsp_i2c_bus_t *bus)
{ return bus == NULL ? HAL_ERROR : bus->init_status; }
/** 返回当前总线最近一次底层操作结果，bus=NULL 返回 HAL_ERROR。
 * 上层参数检查未调用总线时，此值不会刷新。 */
HAL_StatusTypeDef bsp_i2c_get_last_status(const bsp_i2c_bus_t *bus)
{ return bus == NULL ? HAL_ERROR : bus->last_status; }
/** 返回最近底层 HAL 错误位，bus=NULL 返回 NONE。
 * 必须结合状态判断，NONE 不表示上层 EEPROM 操作一定成功。 */
uint32_t bsp_i2c_get_last_error(const bsp_i2c_bus_t *bus)
{ return bus == NULL ? HAL_I2C_ERROR_NONE : bus->last_error; }
