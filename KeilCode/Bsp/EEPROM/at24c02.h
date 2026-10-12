/**
 * @file at24c02.h
 * @brief 本板24C02字节读写接口，直接使用CubeMX句柄hi2c2。
 */
#ifndef AT24C02_H
#define AT24C02_H

#include "bsp_i2c.h"

#define AT24C02_CAPACITY       256U  /* 字节容量：内部地址0x00～0xFF。 */
#define AT24C02_ADDRESS        0x50U /* 7位器件地址，传给HAL时左移1位。 */
#define AT24C02_TIMEOUT_MS     100U  /* 单次HAL数据传输超时。 */
#define AT24C02_WRITE_WAIT_MS  20U   /* 每字节内部写周期的等待预算。 */
#define AT24C02_TEST_ADDRESS   0x20U /* 字符串测试区，避开PID的0x00～0x17。 */

/* 前提：已调用bsp_I2C2_Start；仅在前台串行调用，不在中断中使用。
 * address是芯片内部地址，length是字节数，返回0成功、1失败。
 * write的data是输入；read的data是输出缓冲区，失败时可能部分更新。
 * 写入按字节等待完成；发生失败时已写入的数据不回滚。
 * 零长度不进行通信；两个接口均不自动打印、不自动初始化。 */
uint8_t at24c02_write(uint16_t address, const uint8_t *data, uint16_t length);
uint8_t at24c02_read(uint16_t address, uint8_t *data, uint16_t length);

#endif
