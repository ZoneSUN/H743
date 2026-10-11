/**
 * @file BRT38Encoder_Process.c
 * @brief BRT38 ???????????? PendSV ?????? UART ??? 
 */
#include "BRT38Encoder_Process.h"
#include "USART2_485_RxTx.h"
#include "stm32h7xx.h"
#include "string.h"

/* Info.c ????????????? BRT38_EncoderPacket_TakeReady? */
extern uint8_t BRT38_EncoderFrame_TakeReady(void);

#define BRT38_FWD_MAX                     64U

static BRT38_JobFn s_job[BRT38_JOB_MAX];
static uint8_t s_job_count = 0U;
static uint8_t s_crc_ok = 0U;
static uint8_t s_job_snap[BRT38_ENCODER_FRAME_MAX_DATA];
/* ?? usart2_rx_buf(0x24000420) ???DMA1 ???? */
static uint8_t s_brt38_fwd_buf[BRT38_FWD_MAX] __attribute__((at(0x24000440), zero_init));

static uint16_t BRT38_ModbusCrc(const uint8_t *data, uint16_t len)
{
  uint16_t crc = 0xFFFFU;
  uint16_t i;
  uint8_t bit;

  for (i = 0U; i < len; i++)
  {
    crc ^= data[i];
    for (bit = 0U; bit < 8U; bit++)
    {
      if ((crc & 0x0001U) != 0U)
      {
        crc = (uint16_t)((crc >> 1) ^ 0xA001U);
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return crc;
}

static uint8_t BRT38_CrcOk(const BRT38_ENCODER_FRAME_INFO_t *pkt)
{
  uint16_t crc;
  uint16_t rx;

  if ((pkt == NULL) || (pkt->data == NULL) || (pkt->len < 4U))
  {
    return 0U;
  }
  crc = BRT38_ModbusCrc(pkt->data, (uint16_t)(pkt->len - 2U));
  rx = (uint16_t)pkt->data[pkt->len - 2U] | ((uint16_t)pkt->data[pkt->len - 1U] << 8);
  return (crc == rx) ? 1U : 0U;
}

static uint16_t BRT38_Be16(const uint8_t *p)
{
  return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static uint32_t BRT38_Be32(const uint8_t *p)
{
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

void BRT38_Job_Register(BRT38_JobFn fn)
{
  if ((fn == NULL) || (s_job_count >= BRT38_JOB_MAX))
  {
    return;
  }
  s_job[s_job_count] = fn;
  s_job_count++;
}

void BRT38_Packet_Verify(const BRT38_ENCODER_FRAME_INFO_t *pkt)
{
  s_crc_ok = BRT38_CrcOk(pkt);
}

void BRT38_Packet_Parse(const BRT38_ENCODER_FRAME_INFO_t *pkt)
{
  BRT38_ENCODER_INFO_t info;
  const uint8_t *payload;
  uint8_t nbytes;

  memset(&info, 0, sizeof(info));
  if ((s_crc_ok == 0U) && (BRT38_CrcOk(pkt) == 0U))
  {
    BRT38_EncoderInfo_Commit(&info);
    return;
  }

  info.addr = pkt->data[0];
  info.func = pkt->data[1];
  info.valid = 1U;
  if ((info.func & 0x80U) != 0U)
  {
    info.exception = pkt->data[2];
    BRT38_EncoderInfo_Commit(&info);
    return;
  }
  if ((info.func != 0x03U) || (pkt->len < 5U))
  {
    BRT38_EncoderInfo_Commit(&info);
    return;
  }

  nbytes = pkt->data[2];
  if ((uint16_t)(3U + nbytes + 2U) > pkt->len)
  {
    info.valid = 0U;
    BRT38_EncoderInfo_Commit(&info);
    return;
  }

  payload = &pkt->data[3];
  if (nbytes >= 8U)
  {
    info.encoder_value = BRT38_Be32(payload);
    info.turn_count = BRT38_Be16(&payload[4]);
    info.single_turn = BRT38_Be16(&payload[6]);
    info.value_valid = 1U;
    info.turn_valid = 1U;
    info.single_valid = 1U;
  }
  else if (nbytes == 4U)
  {
    info.encoder_value = BRT38_Be32(payload);
    info.value_valid = 1U;
  }
  else if (nbytes == 2U)
  {
    info.reg_value = BRT38_Be16(payload);
    info.reg_valid = 1U;
  }
  BRT38_EncoderInfo_Commit(&info);
}

void BRT38_Packet_Forward(const BRT38_ENCODER_FRAME_INFO_t *pkt)
{
  uint16_t n;

  if ((pkt == NULL) || (pkt->data == NULL) || (pkt->len == 0U))
  {
    return;
  }
  if ((usart2_485_handle.gState != HAL_UART_STATE_READY) || (usart2_485_handle.hdmatx == NULL))
  {
    return;
  }
  n = pkt->len;
  if (n > BRT38_FWD_MAX)
  {
    n = BRT38_FWD_MAX;
  }
  memcpy(s_brt38_fwd_buf, pkt->data, n);
  (void)USART2_485_SendDataDMA(&usart2_485_handle, s_brt38_fwd_buf, n, 0U);
}

void BRT38_Job_BindDefault(void)
{
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  s_job_count = 0U;
  BRT38_Job_Register(BRT38_Packet_Verify);
  BRT38_Job_Register(BRT38_Packet_Parse);
  BRT38_Job_Register(BRT38_Packet_Forward);
}

void BRT38_Job_Post(void)
{
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void BRT38_Job_Dispatch(void)
{
  uint8_t i;

  while (BRT38_EncoderFrame_TakeReady() != 0U)
  {
    const BRT38_ENCODER_FRAME_INFO_t *src = BRT38_EncoderPacket_Get();
    BRT38_ENCODER_FRAME_INFO_t view;
    uint16_t n = 0U;

    if ((src != NULL) && (src->data != NULL))
    {
      n = src->len;
      if (n > BRT38_ENCODER_FRAME_MAX_DATA)
      {
        n = BRT38_ENCODER_FRAME_MAX_DATA;
      }
      memcpy(s_job_snap, src->data, n);
    }
    view.data = s_job_snap;
    view.len = n;
    s_crc_ok = 0U;
    for (i = 0U; i < s_job_count; i++)
    {
      if (s_job[i] != NULL)
      {
        s_job[i](&view);
      }
    }
  }
}
