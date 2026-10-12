/**
 * @file at24c02.c
 * @brief 直接调用HAL完成24C02读写，不使用配置对象和多层转发。
 */
#include "at24c02.h"

/**
 * @brief 连续读取length字节，结果写入data。
 * @note 24C02内部地址宽度为8位；HAL负责地址发送、重复起始和ACK/NACK。
 */
uint8_t at24c02_read(uint16_t address, uint8_t *data, uint16_t length)
{
    if (address >= AT24C02_CAPACITY || length > AT24C02_CAPACITY - address ||
        (data == NULL && length != 0U))
    {
        return 1U;
    }
    if (length == 0U)
    {
        return 0U;
    }
    return HAL_I2C_Mem_Read(&hi2c2, AT24C02_ADDRESS << 1, address,
                           I2C_MEMADD_SIZE_8BIT, data, length,
                           AT24C02_TIMEOUT_MS) == HAL_OK ? 0U : 1U;
}

/**
 * @brief 将data的length字节写到address开始的位置。
 * @note 逐字节写入避免页大小及跨页问题；每字节STOP后等待器件再次ACK。
 *       HAL发送成功只表示总线传输完成，器件再次ACK才表示可继续访问。
 *       等待使用tick差值，兼容计数回绕；写入中途失败不回滚。
 */
uint8_t at24c02_write(uint16_t address, const uint8_t *data, uint16_t length)
{
    uint16_t index;
    uint32_t start;
    HAL_StatusTypeDef status;

    if (address >= AT24C02_CAPACITY || length > AT24C02_CAPACITY - address ||
        (data == NULL && length != 0U))
    {
        return 1U;
    }
    for (index = 0U; index < length; index++)
    {
        if (HAL_I2C_Mem_Write(&hi2c2, AT24C02_ADDRESS << 1, address + index,
                              I2C_MEMADD_SIZE_8BIT, (uint8_t *)&data[index],
                              1U, AT24C02_TIMEOUT_MS) != HAL_OK)
        {
            return 1U;
        }
        /* EEPROM内部写入期间可能NACK；只等待写周期，不重复发送数据。 */
        start = HAL_GetTick();
        do
        {
            status = HAL_I2C_IsDeviceReady(&hi2c2, AT24C02_ADDRESS << 1, 1U, 1U);
            if (status == HAL_OK)
            {
                break;
            }
            if (status != HAL_ERROR ||
                (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF &&
                 HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_TIMEOUT) ||
                (uint32_t)(HAL_GetTick() - start) >= AT24C02_WRITE_WAIT_MS)
            {
                return 1U;
            }
            HAL_Delay(1U);
        } while (1);
    }
    return 0U;
}
