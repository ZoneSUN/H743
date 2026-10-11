/** @file at24c02.h
 * @brief 24C02 芯片驱动。只管理字节存储，不包含 PID 或业务存储布局。
 * 所有接口返回 0 成功、1 失败；前台串行调用，禁止在中断中使用。
 */
#ifndef AT24C02_H
#define AT24C02_H
#include "bsp_IICx_demo.h"
#define AT24C02_CAPACITY 256U /* 字节容量，合法内部地址为 0x00..0xFF。 */
#define AT24C02_ADDRESS_DEFAULT 0x50U /* 默认 7 位器件地址，按实际地址脚调整。 */
#define AT24C02_TIMEOUT_DEFAULT_MS 100U /* 单次 HAL 传输超时，不是整组保存超时。 */
#define AT24C02_WRITE_WAIT_DEFAULT_MS 20U /* 每个字节写周期的 ACK 轮询预算。 */
/* 器件对象绑定总线，同一总线上可以创建不同地址的多个 EEPROM 对象。
 * bus 必须长期有效，address_7bit 对应实际地址脚配置，合法范围 0x50..0x57。
 * timeout_ms：一次传输超时；write_wait_ms：内部写完成轮询预算。 */
typedef struct {
    bsp_i2c_bus_t *bus;       /* 所绑定的长期有效总线对象。 */
    uint16_t address_7bit;   /* 选择哪个芯片，不是内部存储地址。 */
    uint32_t timeout_ms;     /* 每次传输允许阻塞的时间，毫秒。 */
    uint32_t write_wait_ms;  /* 一个字节写入后等待就绪的预算，毫秒。 */
} at24c02_t;
/* 初始化总线，不写 EEPROM；check 单独检查器件 ACK。 */
/** device 指向长期有效器件对象；配置合法后初始化所绑定的硬件总线。
 * 返回 0 成功、1 失败，不探测芯片、不写任何数据。 */
uint8_t at24c02_init(at24c02_t *device);
/** 发送器件地址检查 ACK，返回 0 表示当前应答、1 表示失败。
 * 不发送存储内容，不破坏 EEPROM；需先成功初始化总线。 */
uint8_t at24c02_check(at24c02_t *device);
/* 地址范围 0..255；length 为字节数且不可跨越容量末尾。
 * 零长度不执行传输，允许 data=NULL，但仍检查器件配置与起始地址。
 * read 失败可能部分更新缓冲区；write 失败不回滚已经写入的字节。
 * write 使用逐字节写入并等待 ACK，不实现页写，不自动读回比较。 */
/** 从 address 开始连续读取 length 字节到 data。
 * 24C02 内部地址宽度固定为 8 位；传输失败缓冲区可能已被部分修改。 */
uint8_t at24c02_read(at24c02_t *device, uint16_t address, uint8_t *data, uint16_t length);
/** 先验证完整地址范围，再按字节递增写入并等待完成。
 * 不使用页写，避免跨页处理；遇到错误立即停止，先前写入的数据不回滚。 */
uint8_t at24c02_write(at24c02_t *device, uint16_t address, const uint8_t *data, uint16_t length);
/* 单字节读取失败不改变调用者的输出。 */
/** 单字节读取包装，先读到 temp，成功后赋给输出。
 * data=NULL 或读取失败返回 1，不改变调用者已有值。 */
uint8_t at24c02_read_byte(at24c02_t *device, uint16_t address, uint8_t *data);
/** 写一个字节后等待内部写完成，成功返回 0。
 * HAL 发送完成不等于 EEPROM 内部写完成，因此必须继续 ACK polling。 */
uint8_t at24c02_write_byte(at24c02_t *device, uint16_t address, uint8_t data);
#endif
