/**
 * @file BRT38_Info.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-10-06
 *
 * @copyright Copyright (c) 2026
 * BRT38拉线编码器设备信息
 *
 * RS485 协议为 Modbus-RTU。0x03 应答里的寄存器按大端存放。
 * 多寄存器应答按从 0x0000 起连续读取来切分字段。
 */

#ifndef __BRT38_ENCODER_INFO_H
#define __BRT38_ENCODER_INFO_H

#include "stdint.h"

#define BRT38_ENCODER_FRAME_MAX_DATA      1024U

/* 解析后的测量值。valid 为 0 时其余字段无效。 */
typedef struct{
    uint8_t  addr;             /* 从站地址 */
    uint8_t  func;             /* 功能码，异常时为 0x80|功能码 */
    uint8_t  valid;            /* 1: 最近一帧 CRC 通过 */
    uint8_t  exception;        /* Modbus 异常码，0 表示无异常 */
    uint32_t encoder_value;    /* 0x0000~0x0001 编码器值 */
    uint16_t turn_count;       /* 0x0002 圈数 */
    uint16_t single_turn;      /* 0x0003 单圈值 */
    uint16_t reg_value;        /* 仅 2 字节应答时的寄存器原值 */
    uint8_t  value_valid;
    uint8_t  turn_valid;
    uint8_t  single_valid;
    uint8_t  reg_valid;
}BRT38_ENCODER_INFO_t;

/* 原始帧: len 为当前长度, data 指向缓冲 */
typedef struct{
    uint16_t len;
    uint8_t *data;
}BRT38_ENCODER_FRAME_INFO_t;



/**
 * @brief 更新编码器数据包
 * @param data 数据包指针
 * @param len 数据包长度
 */
void BRT38_EncoderPacket_Update(uint8_t *data, uint16_t len);

/**
 * @brief 获取编码器数据包
 * @return 编码器数据包指针
 */
BRT38_ENCODER_FRAME_INFO_t *BRT38_EncoderPacket_Get(void);

/**
 * @brief 获取编码器数据包是否准备好
 * @return 1: 准备好, 0: 未准备好
 */
uint8_t BRT38_EncoderPacket_TakeReady(void);

/**
 * @brief 获取编码器数据包信息
 * @return 编码器数据包信息指针
 */
const BRT38_ENCODER_INFO_t *BRT38_EncoderInfo_Get(void);


/**
 * @brief 提交编码器数据包信息
 * @param info 编码器数据包信息指针
 */
void BRT38_EncoderInfo_Commit(const BRT38_ENCODER_INFO_t *info);

#endif /* __BRT38_ENCODER_INFO_H */
