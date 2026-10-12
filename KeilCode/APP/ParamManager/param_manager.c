/**
 * @file param_manager.c
 * @brief PID参数按固定地址逐项读写，不再绑定管理对象或额外状态。
 */
#include "param_manager.h"
#include <math.h>

typedef char float_must_be_4_bytes[(sizeof(float) == PARAM_FLOAT_SIZE) ? 1 : -1];

const param_pid_t PARAM_PID_DEFAULTS =
{
    PARAM_PID_DEFAULT_KP,
    PARAM_PID_DEFAULT_KI,
    PARAM_PID_DEFAULT_KD,
    PARAM_PID_DEFAULT_TARGET,
    PARAM_PID_DEFAULT_OUTPUT_MIN,
    PARAM_PID_DEFAULT_OUTPUT_MAX
};

/* 输入为待保存参数；每个字段4字节，保持原EEPROM数据布局。
 * uint8_t指针用于访问float原始字节，不直接写整个结构体的布局。
 * 某项失败即停止；合法性检查只保留有限值和输出上下限关系。 */
uint8_t param_manager_save_pid(const param_pid_t *params)
{
    if (params == NULL || !isfinite(params->kp) || !isfinite(params->ki) ||
        !isfinite(params->kd) || !isfinite(params->target) ||
        !isfinite(params->output_min) || !isfinite(params->output_max) ||
        params->output_min > params->output_max)
    {
        return 1U;
    }
    return (at24c02_write(PARAM_PID_KP_ADDR, (const uint8_t *)&params->kp, PARAM_FLOAT_SIZE) ||
            at24c02_write(PARAM_PID_KI_ADDR, (const uint8_t *)&params->ki, PARAM_FLOAT_SIZE) ||
            at24c02_write(PARAM_PID_KD_ADDR, (const uint8_t *)&params->kd, PARAM_FLOAT_SIZE) ||
            at24c02_write(PARAM_PID_TARGET_ADDR, (const uint8_t *)&params->target, PARAM_FLOAT_SIZE) ||
            at24c02_write(PARAM_PID_OUTPUT_MIN_ADDR, (const uint8_t *)&params->output_min, PARAM_FLOAT_SIZE) ||
            at24c02_write(PARAM_PID_OUTPUT_MAX_ADDR, (const uint8_t *)&params->output_max, PARAM_FLOAT_SIZE)) ? 1U : 0U;
}

/* 输出为实际读取的参数；读取失败不改变调用者的结构体。
 * EEPROM保存本平台float字节，小端STM32下仍与原布局兼容。 */
uint8_t param_manager_load_pid(param_pid_t *params)
{
    param_pid_t temp;
    if (params == NULL)
    {
        return 1U;
    }
    if (at24c02_read(PARAM_PID_KP_ADDR, (uint8_t *)&temp.kp, PARAM_FLOAT_SIZE) ||
        at24c02_read(PARAM_PID_KI_ADDR, (uint8_t *)&temp.ki, PARAM_FLOAT_SIZE) ||
        at24c02_read(PARAM_PID_KD_ADDR, (uint8_t *)&temp.kd, PARAM_FLOAT_SIZE) ||
        at24c02_read(PARAM_PID_TARGET_ADDR, (uint8_t *)&temp.target, PARAM_FLOAT_SIZE) ||
        at24c02_read(PARAM_PID_OUTPUT_MIN_ADDR, (uint8_t *)&temp.output_min, PARAM_FLOAT_SIZE) ||
        at24c02_read(PARAM_PID_OUTPUT_MAX_ADDR, (uint8_t *)&temp.output_max, PARAM_FLOAT_SIZE))
    {
        return 1U;
    }
    if (!isfinite(temp.kp) || !isfinite(temp.ki) || !isfinite(temp.kd) ||
        !isfinite(temp.target) || !isfinite(temp.output_min) ||
        !isfinite(temp.output_max) || temp.output_min > temp.output_max)
    {
        return 1U;
    }
    *params = temp;
    return 0U;
}
