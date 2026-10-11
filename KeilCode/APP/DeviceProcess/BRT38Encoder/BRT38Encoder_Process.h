/**
 * @file BRT38Encoder_Process.h
 * @brief 包更新之后的校验、解析、透传。不在 UART 回调里执行。
 *
 * UART7 空闲回调在 BRT38_EncoderPacket_Update 之后调用 BRT38_Job_Post。
 * PendSV_Handler 里调用 BRT38_Job_Dispatch。
 * main 上电调用一次 BRT38_Job_BindDefault，while 里不调用。
 */
#ifndef __BRT38_ENCODER_PROCESS_H
#define __BRT38_ENCODER_PROCESS_H

#include "BRT38Encoder_Info.h"

#define BRT38_JOB_MAX                     3U

/* 定义一个处理函数类型 */
typedef void (*BRT38_JobFn)(const BRT38_ENCODER_FRAME_INFO_t *pkt);

/* 注册一个处理函数。BindDefault 会注册校验、解析、透传。 */
void BRT38_Job_Register(BRT38_JobFn fn);

/* 设置 PendSV 优先级 15，并注册三个默认处理函数。 */
void BRT38_Job_BindDefault(void);

/* 回调末尾调用：挂起 PendSV，函数本身立即返回。 */
void BRT38_Job_Post(void);

/* PendSV_Handler 中调用：取出已更新的包，再依次调用已注册函数。 */
void BRT38_Job_Dispatch(void);

void BRT38_Packet_Verify(const BRT38_ENCODER_FRAME_INFO_t *pkt);
void BRT38_Packet_Parse(const BRT38_ENCODER_FRAME_INFO_t *pkt);
void BRT38_Packet_Forward(const BRT38_ENCODER_FRAME_INFO_t *pkt);

#endif /* __BRT38_ENCODER_PROCESS_H */
