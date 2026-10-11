/** @file at24c02.c
 * @brief 24C02 地址检查、硬件 I2C 访问和写周期等待。
 * Copyright (c) 2020-2032, MZXQ. Ported from original 24cxx.c.
 */
#include "at24c02.h"
/** 检查器件指针、总线绑定、7 位地址范围和非零超时。仅检查配置，不进行通信。
 * address_7bit=0x50..0x57 由硬件地址脚决定，不能传已左移地址。 */
static uint8_t valid(const at24c02_t *device)
{
    return device != NULL && device->bus != NULL &&
        device->address_7bit >= 0x50U && device->address_7bit <= 0x57U &&
        device->timeout_ms != 0U && device->write_wait_ms != 0U;
}
/** 检查起始地址和字节长度，确保完整操作位于 256 字节内。
 * 非零长度要求缓冲区有效，零长度可传 NULL；短路判断避免地址减法下溢。 */
static uint8_t range_valid(uint16_t address, const void *data, uint16_t length)
{
    /* 先验证起点，再做减法，防止无符号下溢。 */
    return address < AT24C02_CAPACITY && length <= AT24C02_CAPACITY - address &&
        (data != NULL || length == 0U);
}
/** device 指向长期有效器件对象；配置合法后初始化所绑定的硬件总线。
 * 返回 0 成功、1 失败，不探测芯片、不写任何数据。 */
uint8_t at24c02_init(at24c02_t *device)
{ return valid(device) && bsp_i2c_init(device->bus) == HAL_OK ? 0U : 1U; }
/** 发送器件地址检查 ACK，返回 0 表示当前应答、1 表示失败。
 * 不发送存储内容，不破坏 EEPROM；需先成功初始化总线。 */
uint8_t at24c02_check(at24c02_t *device)
{
    return valid(device) && bsp_i2c_is_device_ready(device->bus,
        device->address_7bit, 1U, device->timeout_ms) == HAL_OK ? 0U : 1U;
}
/* STOP 后器件开始内部写周期；地址 ACK 表示重新就绪。
 * 轮询预算并非整个缓冲区写入的超时，每一个字节都独立等待。
 * 最后一轮 HAL 调用与 1 ms 延时可使实际耗时略超出预算。 */
/** 等待一个字节的内部写周期结束。用 tick 差值避免计数器回绕问题。
 * 返回 0 已就绪、1 超时或总线错误；只重试 HAL 对 NACK 使用的错误组合。 */
static uint8_t wait_complete(at24c02_t *device)
{
    uint32_t start = HAL_GetTick();
    HAL_StatusTypeDef status;
    uint32_t error;
    do {
        status = bsp_i2c_is_device_ready(device->bus, device->address_7bit, 1U, 1U);
        if (status == HAL_OK) return 0U;
        error = bsp_i2c_get_last_error(device->bus);
        /* 本工程 HAL 的 NACK 重试耗尽可能返回 ERROR+TIMEOUT；允许继续轮询。
         * BUSY、真正 HAL_TIMEOUT、BERR/ARLO 等立即退出并保留总线错误。 */
        if (status != HAL_ERROR ||
            (error != HAL_I2C_ERROR_AF && error != HAL_I2C_ERROR_TIMEOUT)) return 1U;
        if ((uint32_t)(HAL_GetTick() - start) >= device->write_wait_ms) return 1U;
        HAL_Delay(1U); /* 使用差值判断，兼容 HAL tick 自然回绕。 */
    } while (1);
}
/** 从 address 开始连续读取 length 字节到 data。
 * 24C02 内部地址宽度固定为 8 位；传输失败缓冲区可能已被部分修改。 */
uint8_t at24c02_read(at24c02_t *device, uint16_t address, uint8_t *data, uint16_t length)
{
    if (!valid(device) || !range_valid(address, data, length)) return 1U;
    return bsp_i2c_mem_read(device->bus, device->address_7bit, address,
        I2C_MEMADD_SIZE_8BIT, data, length, device->timeout_ms) == HAL_OK ? 0U : 1U;
}
/** 写一个字节后等待内部写完成，成功返回 0。
 * HAL 发送完成不等于 EEPROM 内部写完成，因此必须继续 ACK polling。 */
uint8_t at24c02_write_byte(at24c02_t *device, uint16_t address, uint8_t data)
{
    if (!valid(device) || address >= AT24C02_CAPACITY) return 1U;
    if (bsp_i2c_mem_write(device->bus, device->address_7bit, address,
        I2C_MEMADD_SIZE_8BIT, &data, 1U, device->timeout_ms) != HAL_OK) return 1U;
    return wait_complete(device);
}
/** 先验证完整地址范围，再按字节递增写入并等待完成。
 * 不使用页写，避免跨页处理；遇到错误立即停止，先前写入的数据不回滚。 */
uint8_t at24c02_write(at24c02_t *device, uint16_t address, const uint8_t *data, uint16_t length)
{
    uint16_t index;
    if (!valid(device) || !range_valid(address, data, length)) return 1U;
    /* 零长度也通过总线访问校验，保证未初始化时不会返回虚假成功。 */
    if (length == 0U)
        return bsp_i2c_mem_write(device->bus, device->address_7bit, address,
            I2C_MEMADD_SIZE_8BIT, data, 0U, device->timeout_ms) == HAL_OK ? 0U : 1U;
    for (index = 0U; index < length; index++)
        if (at24c02_write_byte(device, (uint16_t)(address + index), data[index]) != 0U) return 1U;
    return 0U;
}
/** 单字节读取包装，先读到 temp，成功后赋给输出。
 * data=NULL 或读取失败返回 1，不改变调用者已有值。 */
uint8_t at24c02_read_byte(at24c02_t *device, uint16_t address, uint8_t *data)
{
    uint8_t temp;
    if (data == NULL || at24c02_read(device, address, &temp, 1U) != 0U) return 1U;
    *data = temp;
    return 0U;
}
