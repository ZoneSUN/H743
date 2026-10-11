/**
 * @file bsp_IICx_demo.h
 * @brief 通用硬件 I2C 总线接口。目录名按工程规划保留，已不包含 PID 演示。
 * 每条物理总线对应一个对象；同一总线的多个器件共享该对象。
 * 必须在 HAL_Init 和系统时钟初始化之后调用；阻塞访问仅供前台串行使用。
 * 多任务环境由上层对同一总线加锁，不允许中断与前台同时调用。
 */
#ifndef BSP_IICX_DEMO_H
#define BSP_IICX_DEMO_H
#include "stm32h7xx_hal.h"

/* 板级回调负责时钟源、GPIO 和外设时钟；通用层不固定任何引脚。 */
typedef struct {
    I2C_TypeDef *instance;       /* 硬件外设基址，如 I2C2；每条物理总线使用独立实例。 */
    uint32_t timing;             /* TIMINGR，必须与实际内核时钟及线路上升时间匹配。 */
    uint32_t analog_filter;      /* I2C_ANALOGFILTER_ENABLE / DISABLE。 */
    uint32_t digital_filter;     /* HAL 数字滤波系数，合法范围 0..15。 */
    HAL_StatusTypeDef (*prepare)(void); /* 板级 GPIO/时钟准备，HAL_OK 表示成功。 */
    uint8_t (*clock_valid)(void); /* 时钟变化后拒绝使用旧时序，需重新初始化。 */
} bsp_i2c_config_t;

/* 对象应静态/长期存活并零初始化，config 也必须长期有效。不要复制已初始化对象。 */
typedef struct {
    const bsp_i2c_config_t *config; /* 指向长期有效的只读硬件配置。 */
    I2C_HandleTypeDef handle;       /* HAL 运行状态与外设句柄，由通用初始化维护。 */
    HAL_StatusTypeDef init_status; /* 最近初始化结果，初值应为 HAL_ERROR。 */
    HAL_StatusTypeDef last_status; /* 最近底层传输/检测/初始化状态。 */
    uint32_t last_error;            /* HAL 错误位，需结合 last_status 判断。 */
    uint8_t initialized;           /* 1=初始化完成；0=不可传输或时钟已失效。 */
} bsp_i2c_bus_t;

/* 返回 HAL_OK / HAL_ERROR / HAL_BUSY / HAL_TIMEOUT；初始化不探测或写入器件。 */
HAL_StatusTypeDef bsp_i2c_init(bsp_i2c_bus_t *bus);
/* address 是未左移的 7 位器件地址；memory_size 使用 HAL 的 8BIT/16BIT 常量。
 * length 是字节数，timeout_ms 是毫秒；读取失败可能部分更新 data。
 * 本层只传输数据，不处理 EEPROM 页边界或内部写周期。 */
HAL_StatusTypeDef bsp_i2c_mem_read(bsp_i2c_bus_t *bus, uint16_t address,
    uint16_t memory, uint16_t memory_size, uint8_t *data, uint16_t length, uint32_t timeout_ms);
/** 参数含义同读取接口，data 是只读发送缓冲区。
 * 仅完成总线传输，不负责 EEPROM 的页边界和写周期等待。
 * 返回 HAL 状态；HAL 发送接口缺少 const，转换仅用于调用旧 HAL。 */
HAL_StatusTypeDef bsp_i2c_mem_write(bsp_i2c_bus_t *bus, uint16_t address,
    uint16_t memory, uint16_t memory_size, const uint8_t *data, uint16_t length, uint32_t timeout_ms);
/* 仅检查地址 ACK，不改变器件存储内容；trials 必须大于 0。 */
HAL_StatusTypeDef bsp_i2c_is_device_ready(bsp_i2c_bus_t *bus, uint16_t address,
    uint32_t trials, uint32_t timeout_ms);
/** 返回最近初始化结果；bus=NULL 返回 HAL_ERROR。
 * 未初始化对象的 init_status 应按板级示例设为 HAL_ERROR。 */
HAL_StatusTypeDef bsp_i2c_get_init_status(const bsp_i2c_bus_t *bus);
/** 返回当前总线最近一次底层操作结果，bus=NULL 返回 HAL_ERROR。
 * 上层参数检查未调用总线时，此值不会刷新。 */
HAL_StatusTypeDef bsp_i2c_get_last_status(const bsp_i2c_bus_t *bus);
/** 返回最近底层 HAL 错误位，bus=NULL 返回 NONE。
 * 必须结合状态判断，NONE 不表示上层 EEPROM 操作一定成功。 */
uint32_t bsp_i2c_get_last_error(const bsp_i2c_bus_t *bus);
#endif
