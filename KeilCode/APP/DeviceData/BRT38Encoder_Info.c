/**
 * @file BRT38Encoder_Info.c
 * @brief BRT38 frame singleton
 */
#include "BRT38Encoder_Info.h"
#include "string.h"

static uint8_t s_brt38_buf[BRT38_ENCODER_FRAME_MAX_DATA];
static BRT38_ENCODER_FRAME_INFO_t s_brt38_frame;
static BRT38_ENCODER_INFO_t s_brt38_info;
static volatile uint8_t s_brt38_ready = 0U;

/**
 * @brief 获取编码器数据包
 * @return 编码器数据包指针
 */
BRT38_ENCODER_FRAME_INFO_t *BRT38_EncoderPacket_Get(void) //BRT38 编码器数据包获取
{
  return &s_brt38_frame;//返回编码器数据包指针
}

/**
 * @brief 更新编码器数据包
 * @param data 数据包指针
 * @param len 数据包长度
 */
void BRT38_EncoderPacket_Update(uint8_t *data, uint16_t len) //BRT38 编码器数据包更新
{
  if ((data == NULL) || (len == 0U))
  {
    return;
  }
  if (len > BRT38_ENCODER_FRAME_MAX_DATA) //如果数据长度大于编码器数据包最大长度
  {
    len = BRT38_ENCODER_FRAME_MAX_DATA;//如果数据长度大于编码器数据包最大长度，则截取最大长度
  }
  memset(s_brt38_buf, 0U, BRT38_ENCODER_FRAME_MAX_DATA);//清空编码器数据包缓冲区
  memcpy(s_brt38_buf, data, len);//复制数据到编码器数据包缓冲区
  s_brt38_frame.data = s_brt38_buf;//更新编码器数据包数据
  s_brt38_frame.len = len;//更新编码器数据包长度
  s_brt38_ready = 1U;//更新编码器数据包准备就绪
}

uint8_t BRT38_EncoderFrame_TakeReady(void)
{
  uint8_t ready = s_brt38_ready;
  if (ready != 0U)
  {
    s_brt38_ready = 0U;
  }
  return ready;
}

const BRT38_ENCODER_INFO_t *BRT38_EncoderInfo_Get(void)
{
  return &s_brt38_info;
}

void BRT38_EncoderInfo_Commit(const BRT38_ENCODER_INFO_t *info)
{
  if (info == NULL)
  {
    return;
  }
  s_brt38_info = *info;
}
