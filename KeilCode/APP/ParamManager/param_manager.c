/**
 * @file param_manager.c
 * @brief 使用头文件地址宏逐项保存 PID，避免整块编码与动态偏移计算。
 * float 用 memcpy 转换为字节，不使用结构体布局或强制转换指针。
 * 保存后每项独立读回验证；阻塞访问，不在中断或并发任务中直接调用。
 */
#include "param_manager.h"
#include <string.h>
#include <math.h>

/* 编译期保证每项确实占 4 字节。跨平台移植还需确认 IEEE-754 和字节序。 */
typedef char float_must_be_4_bytes[(sizeof(float) == PARAM_FLOAT_SIZE) ? 1 : -1];
const param_pid_t PARAM_PID_DEFAULTS = {
    PARAM_PID_DEFAULT_KP, PARAM_PID_DEFAULT_KI, PARAM_PID_DEFAULT_KD,
    PARAM_PID_DEFAULT_TARGET, PARAM_PID_DEFAULT_OUTPUT_MIN, PARAM_PID_DEFAULT_OUTPUT_MAX
};

/** 检查管理对象已绑定器件；总线初始化与器件地址合法性由下层检查。 */
static uint8_t manager_valid(const param_manager_t *manager)
{
    return manager != NULL && manager->eeprom != NULL;
}

/** 使用标准浮点分类检查，避免手动解析 IEEE-754 指数位。
 * 本函数不进行 I2C 通信；仅说明参数值是否可用于后续保存和加载。 */
uint8_t param_manager_pid_is_valid(const param_pid_t *params)
{
    return params != NULL && isfinite(params->kp) && isfinite(params->ki) &&
        isfinite(params->kd) && isfinite(params->target) &&
        isfinite(params->output_min) && isfinite(params->output_max) &&
        params->output_min <= params->output_max;
}

/** 初始化总线后检测芯片应答；失败立即返回，不自动写默认参数。 */
uint8_t param_manager_init(param_manager_t *manager)
{
    if (!manager_valid(manager) || at24c02_init(manager->eeprom) != 0U) return 1U;
    return at24c02_check(manager->eeprom);
}

/** 默认参数只复制到调用者 RAM，保存时机由业务代码决定。 */
void param_manager_get_defaults(param_pid_t *params)
{
    if (params != NULL) *params = PARAM_PID_DEFAULTS;
}

/** 保存一个 float：合法性检查 -> 复制 4 字节 -> 写入 -> 读回比较。
 * address 通常直接使用 PARAM_PID_*_ADDR，容量检查由 at24c02 完成。
 * 比较原始字节而不是近似浮点值，检测传输/存储内容是否完全一致。
 */
uint8_t param_manager_write_float(param_manager_t *manager, uint16_t address, float value)
{
    uint8_t expected[PARAM_FLOAT_SIZE], actual[PARAM_FLOAT_SIZE];
    if (!manager_valid(manager) || !isfinite(value)) return 1U;
    memcpy(expected, &value, PARAM_FLOAT_SIZE);
    if (at24c02_write(manager->eeprom, address, expected, PARAM_FLOAT_SIZE) != 0U) return 1U;
    if (at24c02_read(manager->eeprom, address, actual, PARAM_FLOAT_SIZE) != 0U) return 1U;
    return memcmp(expected, actual, PARAM_FLOAT_SIZE) == 0 ? 0U : 1U;
}

/** 先读取到临时字节和 float，成功且数值有限才更新输出。
 * 不直接读到 value，确保失败时调用者原值保持不变。
 */
uint8_t param_manager_read_float(param_manager_t *manager, uint16_t address, float *value)
{
    uint8_t bytes[PARAM_FLOAT_SIZE];
    float temp;
    if (!manager_valid(manager) || value == NULL) return 1U;
    if (at24c02_read(manager->eeprom, address, bytes, PARAM_FLOAT_SIZE) != 0U) return 1U;
    memcpy(&temp, bytes, PARAM_FLOAT_SIZE);
    if (!isfinite(temp)) return 1U;
    *value = temp;
    return 0U;
}

/** 六个地址与字段一一对应，新增参数时在头文件定义地址并添加相应调用。
 * || 使用短路逻辑：某项失败立即停止，已经完成的写入不回滚。
 */
uint8_t param_manager_save_pid(param_manager_t *manager, const param_pid_t *params)
{
    if (!param_manager_pid_is_valid(params)) return 1U;
    if (param_manager_write_float(manager, PARAM_PID_KP_ADDR, params->kp) ||
        param_manager_write_float(manager, PARAM_PID_KI_ADDR, params->ki) ||
        param_manager_write_float(manager, PARAM_PID_KD_ADDR, params->kd) ||
        param_manager_write_float(manager, PARAM_PID_TARGET_ADDR, params->target) ||
        param_manager_write_float(manager, PARAM_PID_OUTPUT_MIN_ADDR, params->output_min) ||
        param_manager_write_float(manager, PARAM_PID_OUTPUT_MAX_ADDR, params->output_max)) return 1U;
    return 0U;
}

/** 使用相同地址宏加载各字段，最终检查整组上下限关系。
 * candidate 是临时结果，任何字段读取失败都不会部分更新调用者 params。
 */
uint8_t param_manager_load_pid(param_manager_t *manager, param_pid_t *params)
{
    param_pid_t candidate;
    if (params == NULL) return 1U;
    if (param_manager_read_float(manager, PARAM_PID_KP_ADDR, &candidate.kp) ||
        param_manager_read_float(manager, PARAM_PID_KI_ADDR, &candidate.ki) ||
        param_manager_read_float(manager, PARAM_PID_KD_ADDR, &candidate.kd) ||
        param_manager_read_float(manager, PARAM_PID_TARGET_ADDR, &candidate.target) ||
        param_manager_read_float(manager, PARAM_PID_OUTPUT_MIN_ADDR, &candidate.output_min) ||
        param_manager_read_float(manager, PARAM_PID_OUTPUT_MAX_ADDR, &candidate.output_max)) return 1U;
    if (!param_manager_pid_is_valid(&candidate)) return 1U;
    *params = candidate;
    return 0U;
}
