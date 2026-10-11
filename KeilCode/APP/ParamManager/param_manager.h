/**
 * @file param_manager.h
 * @brief 参数结构、默认值、固定存储地址、float 读写与整组 PID 保存/加载。
 * 调用关系：业务 -> 参数管理 -> at24c02 -> 通用硬件 I2C。
 * 状态接口统一返回 0 成功、1 失败；初始化后在前台串行调用。
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

/* 只绑定 EEPROM，不再引入动态基址，地址统一由宏管理。
 * eeprom 必须长期有效；器件选择哪条总线在 at24c02_t 内配置。 */
typedef struct {
    at24c02_t *eeprom;
} param_manager_t;

/* 初始化硬件总线并检查芯片 ACK，不覆盖已有数据、不保存默认值。 */
uint8_t param_manager_init(param_manager_t *manager);
/* 复制默认参数到 RAM，params=NULL 时不操作；不访问 EEPROM。 */
void param_manager_get_defaults(param_pid_t *params);
/* 所有值有限且 min<=max 返回 1，否则返回 0；不限制各增益的业务范围。 */
uint8_t param_manager_pid_is_valid(const param_pid_t *params);

/* 单个 float 接口：直接传地址宏，例如
 * param_manager_write_float(&manager, PARAM_PID_KP_ADDR, 1.5F);
 * param_manager_read_float(&manager, PARAM_PID_KP_ADDR, &kp);
 * 地址必须留足 4 字节；写入会读回比较，读取失败不改变输出。
 * 单字段接口只检查有限值，不检查其他字段，因此整组 PID 优先用 save/load。
 */
uint8_t param_manager_write_float(param_manager_t *manager, uint16_t address, float value);
uint8_t param_manager_read_float(param_manager_t *manager, uint16_t address, float *value);
/* 按六个地址宏依次写入并验证；失败可能留下部分新参数，不回滚。
 * 无 CRC、版本或双副本，不具备整组掉电原子性。 */
uint8_t param_manager_save_pid(param_manager_t *manager, const param_pid_t *params);
/* 按地址宏逐项读取到临时结构，全部读取和合法性检查成功才更新 params。
 * 未初始化或损坏的数据可能不合法；合法性检查不能识别所有存储损坏。 */
uint8_t param_manager_load_pid(param_manager_t *manager, param_pid_t *params);
#endif
