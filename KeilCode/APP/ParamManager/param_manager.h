/**
 * @file param_manager.h
 * @brief 参数结构、默认值、固定存储地址、float 读写与整组 PID 保存/加载。
 * 调用关系：业务 -> 参数管理 -> at24c02 -> HAL硬件I2C2。
 * 状态接口统一返回 0 成功、1 失败；启动I2C2后在前台串行调用。
 * 存储使用本平台的 4 字节 IEEE-754 float，小端 STM32H743 与原数据兼容。
 * 修改地址会改变数据布局，旧数据不会自动迁移；新增参数应使用未占用空间。
 */
#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H
#include "at24c02.h"

/* 每个参数占 4 字节，宏值是 EEPROM 内部的绝对起始地址，不是器件地址。
 * 布局集中在这里维护；保存/读取函数直接使用这些宏，不再计算 base+offset。
 * 各参数范围不可重叠，也不能超过 0xFF；0x18..0xFF 当前留给后续参数。
 */
#define PARAM_FLOAT_SIZE          4U
#define PARAM_PID_KP_ADDR         0x00U /* kp：0x00..0x03，比例增益。 */
#define PARAM_PID_KI_ADDR         0x04U /* ki：0x04..0x07，积分增益。 */
#define PARAM_PID_KD_ADDR         0x08U /* kd：0x08..0x0B，微分增益。 */
#define PARAM_PID_TARGET_ADDR     0x0CU /* target：0x0C..0x0F，目标值。 */
#define PARAM_PID_OUTPUT_MIN_ADDR 0x10U /* output_min：0x10..0x13，输出下限。 */
#define PARAM_PID_OUTPUT_MAX_ADDR 0x14U /* output_max：0x14..0x17，输出上限。 */

/* 示例默认值沿用之前的测试设置，实际控制参数及单位由业务层决定。 */
#define PARAM_PID_DEFAULT_KP          1.50F
#define PARAM_PID_DEFAULT_KI          0.20F
#define PARAM_PID_DEFAULT_KD          0.05F
#define PARAM_PID_DEFAULT_TARGET      100.0F
#define PARAM_PID_DEFAULT_OUTPUT_MIN  0.0F
#define PARAM_PID_DEFAULT_OUTPUT_MAX  1000.0F

/* RAM 中的参数结构不直接写入 EEPROM：每个字段按上面的地址宏单独存取。 */
typedef struct {
    float kp;         /* 比例增益。 */
    float ki;         /* 积分增益。 */
    float kd;         /* 微分增益。 */
    float target;     /* 目标值，单位由应用决定。 */
    float output_min; /* 控制输出下限，不能大于 output_max。 */
    float output_max; /* 控制输出上限。 */
} param_pid_t;
extern const param_pid_t PARAM_PID_DEFAULTS;

/* 先启动I2C2。两个接口返回0成功、1失败，按字段地址存取六个float。
 * save不自动读取校验；load先读取到临时结构，全部成功后才更新输出。
 * 部分写入失败不回滚，简单固定地址布局不提供整组掉电原子性。 */
uint8_t param_manager_save_pid(const param_pid_t *params);
uint8_t param_manager_load_pid(param_pid_t *params);
#endif
